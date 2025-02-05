#include "StdAfx.h"
#include ".\SGSimpleTerrainPatch.h"
#include ".\TerrainPatch.h"
#include ".\TerrainPatchRes.h"
#include ".\TPatchTriangulationStatic.h"
#include ".\RSimpleTerrainPatch.h"
#include "..\Math\Frustum.h"
#include "..\SceneGraph\SceneGraph.h"
#include ".\TPatchLodTable.h"
#include "..\EditorSupport\TerrainEditor.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(SGSimpleTerrainPatch,SceneNode);

	SGSimpleTerrainPatch::SGSimpleTerrainPatch(void)
	{
		m_pRenderNode=new RSimpleTerrainPatch;
	}

	SGSimpleTerrainPatch::~SGSimpleTerrainPatch(void)
	{
		SAFE_DELETE(m_pRenderNode);
	}

	void SGSimpleTerrainPatch::Serialize(FArchive& ar)
	{
		ar<<m_box.min;
		ar<<m_box.max;
		ar<<m_patchX;
		ar<<m_patchZ;
		m_vertStream.Serialize(ar);
	}
	
	void SGSimpleTerrainPatch::Deserialize(FArchive& ar)
	{
		ar>>m_box.min;
		ar>>m_box.max;
		ar>>m_patchX;
		ar>>m_patchZ;
		m_vertStream.Deserialize(ar);

		//--
		m_pRenderNode->SetAABBox(m_box);
		m_pRenderNode->SetPatchPos(m_patchX,m_patchZ);
		m_pRenderNode->CreateGeom(m_vertStream);
	}

	bool SGSimpleTerrainPatch::IsInFrustum(const Frustum *pFrumstum) const
	{
		return pFrumstum->BoxInFrustum(m_box);
	}

	void SGSimpleTerrainPatch::LoadRes(const TCHAR* szName)
	{
	}

	UINT SGSimpleTerrainPatch::GetNumRenderNode() const
	{
		return 1;
	}

	RenderNode* SGSimpleTerrainPatch::GetRenderNode(UINT i) const
	{
		return m_pRenderNode;
	}

	const Matrix4& SGSimpleTerrainPatch::GetWorldMat() const
	{
		return MathConst::IdentityMat;
	}

	void SGSimpleTerrainPatch::GetBox(AABBox& out) const
	{
		out=m_box;
	}

	IMaterial*	SGSimpleTerrainPatch::GetMtl(int mtlID)
	{
		return NULL;
	}

	void SGSimpleTerrainPatch::Update(float deltaTime,SceneGraph *pSG)
	{
		int sgLod=pSG->CalcDetailLevel(m_box);
		int trnLod=0;
		if(sgLod<=7)
			trnLod=0;
		else if(sgLod<=9)
			trnLod=1;
		else 
			trnLod=2;
		TPatchLodTable::Inst_SimpleTerrain()->SafeSetLod(m_patchX,m_patchZ,trnLod);
	}

	void SGSimpleTerrainPatch::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{
	}

	void SGSimpleTerrainPatch::SetWorldMat(const Matrix4 *pMat)
	{
		ASSERT(0 && _T("cann't set world transform to a terrain patch"));
	}

	void SGSimpleTerrainPatch::CreateGeom(Heightmap* pHMap,RECT& rc)
	{
		//--
		pHMap->BuildAABBox(&m_box,&rc);

		//--
		m_patchX=rc.left/TerrainEditor::ESimplePatchSize;
		m_patchZ=rc.top/TerrainEditor::ESimplePatchSize;

		//--
		m_vertStream.Create(EVType_P,TerrainEditor::ESimplePatchVerts);
		pHMap->FillVertStream_P(&m_vertStream,TerrainEditor::ESimplePatchSideVerts,rc);
	}

}//namespace Cool3D                      