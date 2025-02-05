#include "StdAfx.h"
#include ".\effectquad.h"
#include "..\RenderSys\DynamicMesh.h"
#include "..\RenderSys\RBillboards.h"
#include "..\Math\Transform.h"
#include "EffectPathCtrl.h"
#include "EffectTarget.h"
#include "SGeffectNode.h"
#include "EftQuadDef.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(EffectQuad,EffectBase);

	class EffectQuad::Member
	{
	public:
		tagQuadProp			m_prop;
		DynamicMesh			*m_pRMesh;
		vector<tagState>	m_keyStates;	//每个关键帧的状态
		int					m_curState;		//当前的关键帧状态index
		float				m_stateTime;	//当前状态保持了多少时间
		float				m_tailFadeSpeed;//尾巴上的Quad(第一个后面的所有)alpha降低的速度,
											//是根据尾巴的长度和尾巴间隔时间计算得来的
		float				m_tailDely;		//上次尾巴增加到现在的时间
		tagEftQuad			*m_quads;
		int					m_tailIndex;	//最后一个尾巴quad在数组中的下标

		bool				m_bActive;//现在是否已经激活
		
		Vector3 curRot;	//当前自转
	public:
		Member():curRot(0,0,0)
		{
			m_curState=0;
			m_stateTime=0;
			m_pRMesh=NULL;
			m_tailFadeSpeed=1.0f;
			m_tailDely=0;
			m_quads=NULL;
			m_tailIndex=1;
			m_bActive=true;
		}
		~Member()	
		{
			Destroy();
		}

		void Destroy()
		{
			SAFE_DELETE(m_pRMesh);
			SAFE_FREE(m_quads);
		}

		//增加一个拖尾的quad
		void AddTailQuad()
		{
			if(m_prop.tailSize==1)
				return;
			m_tailDely=0;//清空时间状态

			m_tailIndex++;
			if(m_tailIndex>=m_prop.tailSize)
				m_tailIndex=1;
			m_quads[m_tailIndex]=m_quads[0];//拷贝head quad的状态
		}
		
		//更新尾巴的Fade状态
		void UpdateTailFade(float deltaTime)
		{
			if(m_prop.tailSize==1)
				return;
			Color4f c;
			for(int i=1;i<m_prop.tailSize;i++)
			{
				c=m_quads[i].color;
				c.A-=m_tailFadeSpeed*deltaTime;
				if(c.A<0.0f)
					c.A=0.0f;
				Color4ub cc=c;
				DWORD dc=cc;
				for(int j=0;j<4;j++)
					m_quads[i].color=dc;
			}
		}

		void SetClosingFade(float alpha)
		{
			Color4f c;
			for(int i=0;i<m_prop.tailSize;i++)
			{
				c=m_quads[i].color;
				if(c.A>alpha)
					c.A=alpha;
				Color4ub cc=c;
				DWORD dc=cc;
				for(int j=0;j<4;j++)
					m_quads[i].color=dc;
			}
		}

		void UpdateQuad(const Color4f &color,const Vector2 &size,
			const Vector3 &pos,const Vector3 &rot,tagEftQuad& quad)
		{
			Color4ub tmpC(color);
			quad.color=tmpC;
			quad.pos=pos;
			quad.rot=rot;
			quad.halfSize=size*0.5f;
		}

		void ResetQuadBuffer()
		{
			ASSERT(m_quads!=NULL);
			int bufSize=m_prop.tailSize*sizeof(tagEftQuad);
			memset(m_quads,0,bufSize);
			for(int i=0;i<m_prop.tailSize;i++)
			{
				m_quads[i].eUV.x=1.0f;
				m_quads[i].eUV.y=1.0f;
			}
		}
	};

	EffectQuad::EffectQuad(void)	  
	{								  
		m_p=new Member;
	}

	EffectQuad::~EffectQuad(void)
	{
		PreDestroy();
		delete m_p;
	}

	void EffectQuad::Serialize(FArchive& ar)
	{
		ar.ValueWrite(m_p->m_prop);
		SerialValVector(ar,m_p->m_keyStates);

		EffectBase::Serialize(ar);
	}

	void EffectQuad::Deserialize(FArchive& ar)
	{
		tagQuadProp prop;
		ar.ValueRead(prop);
		vector<tagState> keyStates;
		DeserialValVector(ar,keyStates);

		// 美术提出没有路径的片也需要残影
		////优化:强制将路径为空的Quad的残影数设为1
		//if(prop.szPathName[0]==0||
		//	_tcsicmp(prop.szPathName,_T("none"))==0)
		//	prop.tailSize=1;

		//build
		BeginBuild(prop);
		for(size_t i=0;i<keyStates.size();i++)
			AddKeyState(keyStates[i]);
		EndBuild();

		EffectBase::Deserialize(ar);
	}

	RenderNode* EffectQuad::GetRenderNode(int i)
	{
		return m_p->m_pRMesh;
	}

	void EffectQuad::Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime)
	{
		if(m_p->m_pRMesh==NULL || m_p->m_quads==NULL)
			return;
		//--是否启动
		if(!m_p->m_bActive)
		{
			if(runTime > m_p->m_prop.m_startTime)
				m_p->m_bActive=true;
		}
		if(!m_p->m_bActive)
			return;

		m_p->m_stateTime+=deltaTime;
		//--插值出当前的状态
		m_p->m_curState=AdvanceState(m_p->m_curState,m_p->m_keyStates.size(),
			m_p->m_keyStates[m_p->m_curState].lastTime,m_p->m_stateTime,
			m_p->m_prop.m_loop);
		ASSERT(m_p->m_curState>=0 
			&& m_p->m_curState<(int)m_p->m_keyStates.size());
		tagState curState=m_p->m_keyStates[m_p->m_curState];
		
		if(m_p->m_keyStates.size()>1)//至少有两帧,才进行插值
		{
			int nextState=m_p->m_curState+1;
			bool bLerp=true;
			if(nextState>=(int)m_p->m_keyStates.size())
			{
				if(!m_p->m_prop.m_loop)//已经播放到最后一个状态,而且不是循环播放,则不需插值
					bLerp=false;
				nextState=0;
			}

			if(bLerp)
			{
				tagState& ts=m_p->m_keyStates[nextState];
				float r=m_p->m_stateTime/curState.lastTime;

				D3DXVec4Lerp((Vector4*)&curState.color,(Vector4*)&curState.color,(Vector4*)&ts.color,r);
				D3DXVec2Lerp(&curState.size,&curState.size,&ts.size,r);
				D3DXVec3Lerp(&curState.rotVel,&curState.rotVel,&ts.rotVel,r);
			}
		}

		//--update tail
		if(m_p->m_prop.tailSize>1)
		{
			if(!m_bClosing)
			{
				m_p->m_tailDely+=deltaTime;
				if(m_p->m_tailDely>m_p->m_prop.tailInterval)
				{
					m_p->AddTailQuad();
					m_p->m_tailDely-=m_p->m_prop.tailInterval;
				}
				m_p->UpdateTailFade(deltaTime);
			}//endof if
		}
		
		//--update head quad		
		m_p->curRot+=curState.rotVel*deltaTime;
		
		Vector3 curRot=m_p->curRot;

		Vector3 pos=m_p->m_prop.offset;
		EffectPathCtrl* pPath=pSGNode->GetEffectPath(m_p->m_prop.szPathName);
		if(pPath)
		{
			pos+=pPath->GetCurPos();
			if(m_p->m_prop.faceType==0)
			{
				curRot+=pPath->GetCurRot();//加上路径的旋转
			}
			m_p->m_pRMesh->SetIdentityWorldMat(false);
		}
		else if(m_p->m_prop.tailSize>1)
		{
			const Matrix4& mat = pSGNode->GetWorldMat();
			pos+=Vector3(mat._41,mat._42,mat._43);
			m_p->m_pRMesh->SetIdentityWorldMat(true);
		}

		m_p->UpdateQuad(curState.color,curState.size,
			pos,curRot,m_p->m_quads[0]);

		if(m_bClosing)//如果正在关闭,则将所有quad的alpha降低
		{
			EffectBase::UpdateClosingFade(runTime,m_p->m_prop.m_closeTime);
			m_p->SetClosingFade(m_closingFade);
		}

		//--update render mesh
		CopyQuadArrayToMesh(m_p->m_pRMesh,m_p->m_quads,m_p->m_prop.tailSize,
			pCamera,pSGNode->GetWorldMat(),m_p->m_prop.faceType);
		

		//--update mtl
		EffectBase::Update(pSG,pSGNode,pCamera,deltaTime,runTime);

	}

	void EffectQuad::BeginBuild(const tagQuadProp& prop)
	{
		m_p->m_keyStates.clear();
		m_p->m_curState=0;
		
		m_p->m_prop=prop;
	}

	void EffectQuad::AddKeyState(const tagState& state)
	{
		m_p->m_keyStates.push_back(state);
	}

	void EffectQuad::EndBuild()
	{
		m_p->Destroy();
		//-- 创建一个Quad需要的Mesh
		m_p->m_pRMesh=new DynamicMesh;
		m_p->m_pRMesh->Create(m_p->m_prop.m_groupID,
			EDraw_TriangleList,EVType_PTD,
			m_p->m_prop.tailSize*6);//每个Quad由两个三角形(6个顶点组成)
		VertStream *pVStream=m_p->m_pRMesh->GetVertStream();
		memset(pVStream->GetRawBuffer(),0,pVStream->GetBufferSize());
		m_p->m_pRMesh->SetDrawLimit(0);


		//--计算尾巴alpha降低的速度
		if(m_p->m_prop.tailLife==0.0f)
		{
			if(m_p->m_prop.tailSize>1)
				m_p->m_tailFadeSpeed=1.0f/((m_p->m_prop.tailSize-1)*m_p->m_prop.tailInterval);
			else
				m_p->m_tailFadeSpeed=1.0f;
		}
		else
			m_p->m_tailFadeSpeed=1.0f/m_p->m_prop.tailLife;

		//--分配quad buffer
		int bufSize=m_p->m_prop.tailSize*sizeof(tagEftQuad);
		m_p->m_quads=(tagEftQuad*)malloc(bufSize);
		m_p->ResetQuadBuffer();

		//--初始化head quad
		ResetPlayState();

		ASSERT(m_p->m_keyStates.size()>0);
		tagState& initState=m_p->m_keyStates[0];
		m_p->UpdateQuad(initState.color,initState.size,
			m_p->m_prop.offset,m_p->curRot,m_p->m_quads[0]);
	}

	void EffectQuad::ResetPlayState()
	{
		m_p->m_curState=0;
		m_p->m_stateTime=0;
		
		m_p->curRot=Deg2Rad3f(m_p->m_prop.initRot);

		//--reset尾巴
		m_p->ResetQuadBuffer();
		m_p->m_tailIndex=1;

		m_p->m_bActive=m_p->m_prop.m_bActive;
		if(!m_p->m_bActive)
			m_p->m_pRMesh->SetDrawLimit(0);

		EffectBase::ResetPlayState();
	}

	const TCHAR* EffectQuad::GetName() const
	{
		return m_p->m_prop.m_szName;
	}

	EffectQuad::tagQuadProp EffectQuad::GetProp() const
	{
		return m_p->m_prop;
	}

	int EffectQuad::GetNumStates() const
	{
		return m_p->m_keyStates.size();
	}

	EffectQuad::tagState EffectQuad::GetState(int i) const
	{
		ASSERT(i>=0 && i<GetNumStates());
		return m_p->m_keyStates[i];
	}

	void EffectQuad::Rename(const TCHAR* szNewName)
	{
		_tcscpy_s(m_p->m_prop.m_szName,sizeof(m_p->m_prop.m_szName)/sizeof(TCHAR),szNewName);
	}

	void EffectQuad::SetGroupID(int id)
	{
		m_p->m_prop.m_groupID=id;
		if(m_p->m_pRMesh)
			m_p->m_pRMesh->SetMtlID(id);
	}

	void EffectQuad::SetOffset( const Vector3& pos )
	{
		m_p->m_prop.offset = pos;
	}

	float EffectQuad::GetPlayTime()
	{
		if(m_p->m_prop.m_loop)
			return -1;

		float time=0;
		for(int i=0;i<(int)m_p->m_keyStates.size();i++)
		{
			time+=m_p->m_keyStates[i].lastTime;
		}
		return time;
	}

	bool EffectQuad::GetBox(AABBox& out) const
	{
		Vector3 vRot = Deg2Rad3f(m_p->m_prop.initRot);
		for(int i=0;i<(int)m_p->m_keyStates.size();i++)
		{
			const tagState& state = m_p->m_keyStates[i];
			AABBox box;
			switch( m_p->m_prop.faceType )
			{
			case 0: // 普通片
				{
					Vector3 vHalfSize( state.size.x * 0.5f, state.size.y * 0.5f, 1.0f );
					box.min = -vHalfSize;
					box.max =  vHalfSize;
				}
				break;
			case 1: // 球形billboard
				{
					float fRadius = max( state.size.x, state.size.y ) * 0.5f;
					box.min = Vector3( -fRadius, -fRadius, -fRadius );
					box.max = Vector3(  fRadius,  fRadius,  fRadius );
				}
				break;
			case 2: // 筒行billboard
				{
					Vector3 vHalfSize( state.size.x * 0.5f, state.size.y * 0.5f, state.size.x * 0.5f );
					box.min = -vHalfSize;
					box.max =  vHalfSize;
				}
				break;
			}
			vRot += state.rotVel * state.lastTime;
			Transform trans;
			trans.Translate( m_p->m_prop.offset );
			trans.Rotate( vRot.y, vRot.x, vRot.z );
			box.Transform( *trans.GetMatrix() );
			out.Merge( box );
		}
		return true;
	}

	int EffectQuad::GetLOD() const
	{
		return m_p->m_prop.m_nLOD;
	}
}//namespace Cool3D