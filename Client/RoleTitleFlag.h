#pragma once

class RoleTitleFlag 
{
public:
	RoleTitleFlag(void);
	~RoleTitleFlag(void);

	void Destroy();
	void DrawRoleTitleFlag(RECT rc, float z, const TCHAR* szTitle);
	void DrawRoleTitleFlag(RECT rc, float z, ResTexture* pRes);

	void DrawTeamFlag(RECT rc, float z, const TCHAR* szTeam);
	void DrawTeamFlag(RECT rc, float z, ResTexture* pRes);

    void DrawPicTitle(RECT rc, float z, LPCTSTR szTitlePic);
    void DrawPicTitle(RECT rc, float z, ResTexture* pRes);

	void DrawVipFlag(RECT rc, float z, const TCHAR* szVip);
	void DrawVipFlag(RECT rc, float z, ResTexture* pRes);

	void DrawGodJob(RECT rc, float z, const TCHAR* szGod);
	void DrawGodJob(RECT rc, float z, ResTexture* pRes);

private:
	map<tstring, ResTexture*>			m_mapRes;

	map<tstring, ResTexture*>			m_mapResTeam;

    map<tstring, ResTexture*>           m_mapResPicTitle;

	map<tstring, ResTexture*>           m_mapResPicGodJob;
};