#pragma once
#include "SDK/cdll_int.h"

class IVEngineClient014
{
public:
	virtual int					GetIntersectingSurfaces(const model_t* model, const Vector& vCenter, const float radius, const bool bOnlyVisibleSurfaces, SurfInfo* pInfos, const int nMaxInfos) = 0;
	virtual Vector				GetLightForPoint(const Vector& pos, bool bClamp) = 0;
	virtual void _unk2() = 0;
	virtual const char* ParseFile(const char* data, char* token, int maxlen) = 0;
	virtual void _unk4() = 0;
	virtual void				GetScreenSize(int& width, int& height) = 0;
	virtual void				ServerCmd(const char* szCmdString, bool bReliable = true) = 0;
	virtual void				ClientCmd(const char* szCmdString) = 0;
	virtual bool				GetPlayerInfo(int ent_num, player_info_t* pinfo) = 0;
	virtual int					GetPlayerForUserID(int userID) = 0;
	virtual client_textmessage_t* TextMessageGet(const char* pName) = 0;
	virtual bool				Con_IsVisible(void) = 0;
	virtual int					GetLocalPlayer(void) = 0;
	virtual const model_t* LoadModel(const char* pName, bool bProp = false) = 0;
	virtual float				GetLastTimeStamp(void) = 0;
	virtual CSentence* GetSentence(CAudioSource* pAudioSource) = 0;
	virtual float				GetSentenceLength(CAudioSource* pAudioSource) = 0;
	virtual bool				IsStreaming(CAudioSource* pAudioSource) const = 0;
	virtual void				GetViewAngles(QAngle& va) = 0;
	virtual void				SetViewAngles(QAngle& va) = 0;
	virtual int					GetMaxClients(void) = 0;
	virtual	const char* Key_LookupBinding(const char* pBinding) = 0;
	virtual const char* Key_BindingForKey(ButtonCode_t code) = 0;
	virtual void _unk23() = 0;
	virtual void				StartKeyTrapMode(void) = 0;
	virtual bool				CheckDoneKeyTrapping(ButtonCode_t& code) = 0;
	virtual bool				IsInGame(void) = 0;
	virtual bool				IsConnected(void) = 0;
	virtual void _unk28() = 0;
	virtual void _unk29() = 0;
	virtual void				Con_NPrintf(int pos, const char* fmt, ...) = 0;
	virtual void _unk31() = 0;
	virtual int					IsBoxVisible(const Vector& mins, const Vector& maxs) = 0;
	virtual int					IsBoxInViewCluster(const Vector& mins, const Vector& maxs) = 0;
	virtual bool				CullBox(const Vector& mins, const Vector& maxs) = 0;
	virtual void _unk35() = 0;
	virtual void _unk36() = 0;
	virtual const VMatrix& WorldToScreenMatrix() = 0;
	virtual const VMatrix& WorldToViewMatrix() = 0;
	virtual int					GameLumpVersion(int lumpId) const = 0;
	virtual int					GameLumpSize(int lumpId) const = 0;
	virtual void _unk41() = 0;
	virtual int					LevelLeafCount() const = 0;
	virtual void _unk43() = 0;
	virtual void		LinearToGamma(float* linear, float* gamma) = 0;
	virtual float		LightStyleValue(int style) = 0;
	virtual void _unk46() = 0;
	virtual void		GetAmbientLightColor(Vector& color) = 0;
	virtual void _unk48() = 0;
	virtual void _unk49() = 0;
	virtual void		Mat_Stub(IMaterialSystem* pMatSys) = 0;
	virtual void GetChapterName(char* pchBuff, int iMaxLength) = 0;
	virtual void _unk52() = 0;
	virtual void _unk53() = 0;
	virtual void _unk54() = 0;
	virtual void _unk55() = 0;
	virtual void		EngineStats_BeginFrame(void) = 0;
	virtual void _unk57() = 0;
	virtual void _unk58() = 0;
	virtual void _unk59() = 0;
	virtual int			GetLeavesArea(int* pLeaves, int nLeaves) = 0;
	virtual bool		DoesBoxTouchAreaFrustum(const Vector& mins, const Vector& maxs, int iArea) = 0;
	virtual void _unk62() = 0;
	virtual void _unk63() = 0;
	virtual void _unk64() = 0;
	virtual void _unk65() = 0;
	virtual int			SentenceGroupPickSequential(int groupIndex, char* name, int nameBufLen, int sentenceIndex, int reset) = 0;
	virtual int			SentenceIndexFromName(const char* pSentenceName) = 0;
	virtual const char* SentenceNameFromIndex(int sentenceIndex) = 0;
	virtual int			SentenceGroupIndexFromName(const char* pGroupName) = 0;
	virtual const char* SentenceGroupNameFromIndex(int groupIndex) = 0;
	virtual	float		SentenceLength(int scentenceIndex) = 0;
	virtual void		ComputeLighting(const Vector& pt, const Vector* pNormal, bool bClamp, Vector& color, Vector* pBoxColors = NULL) = 0;
	virtual void _unk73() = 0;
	virtual void _unk74() = 0;
	virtual void _unk75() = 0;
	virtual void* SaveAllocMemory(size_t num, size_t size) = 0;
	virtual void		SaveFreeMemory(void* pSaveMem) = 0;
	virtual INetChannelInfo* GetNetChannelInfo(void) = 0;
	virtual void		DebugDrawPhysCollide(const CPhysCollide* pCollide, IMaterial* pMaterial, matrix3x4_t& transform, const color32& color) = 0;
	virtual void		CheckPoint(const char* pName) = 0;
	virtual void _unk81() = 0;
	virtual bool		IsPlayingDemo(void) = 0;
	virtual bool		IsRecordingDemo(void) = 0;
	virtual bool		IsPlayingTimeDemo(void) = 0;
	virtual int			GetDemoRecordingTick(void) = 0;
	virtual int			GetDemoPlaybackTick(void) = 0;
	virtual int			GetDemoPlaybackStartTick(void) = 0;
	virtual float		GetDemoPlaybackTimeScale(void) = 0;
	virtual int			GetDemoPlaybackTotalTicks(void) = 0;
	virtual bool		IsPaused(void) = 0;
	virtual void _unk91() = 0;
	virtual bool		IsTakingScreenshot() = 0;
	virtual bool		IsHLTV(void) = 0;
	virtual void _unk94() = 0;
	virtual void		GetMainMenuBackgroundName(char* dest, int destlen) = 0;
	virtual void		SetOcclusionParameters(const OcclusionParams_t& params) = 0;
	virtual void		GetUILanguage(char* dest, int destlen) = 0;
	virtual SkyboxVisibility_t IsSkyboxVisibleFromPoint(const Vector& vecPoint) = 0;
	virtual void _unk99() = 0;
	virtual void _unk100() = 0;
	virtual void _unk101() = 0;
	virtual void _unk102() = 0;
	virtual void _unk103() = 0;
	virtual void _unk104() = 0;
	virtual const char* GetProductVersionString() = 0;
	virtual void			GrabPreColorCorrectedFrame(int x, int y, int width, int height) = 0;
	virtual void _unk107() = 0;
	virtual void			ExecuteClientCmd(const char* szCmdString) = 0;
	virtual void _unk109() = 0;
	virtual void _unk110() = 0;
	virtual int	GetAppID() = 0;
	virtual void _unk112() = 0;
	virtual void _unk113() = 0;
	virtual void			ClientCmd_Unrestricted(const char* szCmdString, bool lol = false) = 0;
	virtual void _unk115() = 0;
	virtual void _unk116() = 0;
	virtual void _unk117() = 0;
	virtual void _unk118() = 0;
	virtual void			ReadConfiguration(const bool readDefault = false) = 0;
	virtual void _unk120() = 0;
	virtual IAchievementMgr* GetAchievementMgr() = 0;
	virtual void _unk122() = 0;
	virtual void			SetMapLoadFailed(bool bState) = 0;
	virtual void _unk124() = 0;
	virtual void _unk125() = 0;
	virtual void _unk126() = 0;
	virtual void _unk127() = 0;
	virtual void _unk128() = 0;
	virtual void _unk129() = 0;
	virtual void _unk130() = 0;
	virtual void _unk131() = 0;
	virtual void _unk132() = 0;
	virtual CGamestatsData* GetGamestatsData() = 0;

};