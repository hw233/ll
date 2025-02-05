// EffectAttacherDoc.h :  CEffectAttacherDoc 类的接口
//
#pragma once
#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;


class CEffectAttacherDoc : public CDocument
{
protected: // 仅从序列化创建
	CEffectAttacherDoc();
	DECLARE_DYNCREATE(CEffectAttacherDoc)

// 属性
public:

// 操作
public:

// 重写
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CEffectAttacherDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

public:
	AniMsgTable* GetAniMsgTable();
	CString GetCSVFilePath();
	AniMsgTable m_aniMsgTable;
	CString m_cstrCSVFilePath;
	virtual void OnCloseDocument();
};


