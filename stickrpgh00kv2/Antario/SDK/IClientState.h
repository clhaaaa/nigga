#pragma once

// deadcell https://github.com/EternityX/DEADCELL-CSGO/blob/05c81db8713da0228181fc2342d543028fab97a1/csgo/sdk/interfaces/def.h

class c_clock_drift_mgr
{
public:
	float m_clock_offsets[17];   //0x0000
	uint32_t m_cur_clock_offset; //0x0044
	uint32_t m_server_tick;     //0x0048
	uint32_t m_client_tick;     //0x004C
}; //Size: 0x0050

class IClientState {
public:
public:
	void force_full_update() {
		*reinterpret_cast<int*>(uintptr_t(this) + 0x174) = -1;
	};

	char pad_0000[156];             //0x0000
	NetChannel* m_net_channel;          //0x009C
	uint32_t m_challenge_nr;        //0x00A0
	char pad_00A4[100];             //0x00A4
	uint32_t m_signon_state;        //0x0108
	char pad_010C[8];               //0x010C
	float m_next_cmd_time;          //0x0114
	uint32_t m_server_count;        //0x0118
	uint32_t m_current_sequence;    //0x011C
	char pad_0120[8];               //0x0120
	c_clock_drift_mgr m_clock_drift_mgr; //0x0128
	uint32_t m_delta_tick;          //0x0178
	bool m_paused;                 //0x017C
	char pad_017D[3];               //0x017D
	uint32_t m_view_entity;         //0x0180
	uint32_t m_player_slot;         //0x0184
	char m_level_name[260];        //0x0188
	char m_level_name_short[80];    //0x028C
	char m_group_name[80];         //0x02DC
	char pad_032С[92];				//0x032С
	uint32_t m_max_clients;         //0x0388
	char pad_0314[18820];           //0x0314
	float m_last_server_tick_time;   //0x4C98
	bool m_insimulation;              //0x4C9C
	char pad_4C9D[3];               //0x4C9D
	uint32_t m_oldtickcount;          //0x4CA0
	float m_tick_remainder;          //0x4CA4
	float m_frame_time;              //0x4CA8
	uint32_t m_last_outgoing_command;//0x4CAC
	uint32_t m_choked_commands;     //0x4CB0
	uint32_t m_last_command_ack;     //0x4CB4
	uint32_t m_command_ack;			//0x4CB8
	uint32_t m_sound_sequence;      //0x4CBC
	char pad_4CC0[80];              //0x4CC0
	Vector viewangles;              //0x4D10
};

extern IClientState* g_pClientState;