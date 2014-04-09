#pragma once

/*
	protocol for data transformation

	2014-03-28 file created. base utility added.
*/

enum EProtocol
{
	EPReq_Login,
	EPRes_Login,
	EPReq_Logout,
	EPReq_NormalChat,
	EPRes_NormalChat,
	EPRes_UserList,
	EPRes_UserLogin,
	EPRes_UserLogout,
	EPReq_Wisper,
	EPRes_Wisper,
};
