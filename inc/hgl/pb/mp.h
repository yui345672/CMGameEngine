﻿#ifndef HGL_PROTOCOLBUF_MESSAGE_PROCESS_CREATOR_INCLUDE
#define HGL_PROTOCOLBUF_MESSAGE_PROCESS_CREATOR_INCLUDE

#include<hgl/type/Map.h>
#include<google/protobuf/message_lite.h>

namespace hgl
{
	using namespace google;
	using namespace google::protobuf;

	typedef DefEvent(bool,PBCmdFunc,(MessageLite *));

	/**
	 * ProtocolBuffer消息处理类
	 */
	class PBMessageProc
	{
	private:

		struct PBItem
		{
			MessageLite *ml;
			PBCmdFunc func;

		public:

			PBItem()
			{
				memset(this,0,sizeof(PBItem));
			}

			PBItem(MessageLite *m,const PBCmdFunc &p)
			{
				ml=m;
				func=p;
			}

			const bool operator == (const PBItem &pbi)
			{
				return(ml==pbi.ml);
			}
		};//struct PBItem

		Map<int,PBItem> ml_list;

		int start_cmd_pb;
		int end_cmd_pb;

	public:

		PBMessageProc()
		{
			Clear();
		}

		virtual ~PBMessageProc()
		{
		}

		void Clear()
		{
			start_cmd_pb=end_cmd_pb=-1;
		}

		void Init(int start,int end)
		{
			start_cmd_pb=start;
			end_cmd_pb=end;
		}

		bool SetProc(int id,PBCmdFunc &func,MessageLite *ml)
		{
			if(id<start_cmd_pb||id>end_cmd_pb)RETURN_FALSE;
			if(!ml)RETURN_FALSE;
			if(!func)RETURN_FALSE;

			if(!ml_list.Add(id,PBItem(ml,func)))
			{
				LOG_ERROR(OS_TEXT("repeat add MessageLite,ID:")+OSString(id));
				RETURN_FALSE;
			}

			return(true);
		}

		bool Proc(int id,void *data,int size)
		{
			if(id<start_cmd_pb||id>end_cmd_pb)RETURN_FALSE;

			PBItem pb_item;

			if(!ml_list.Get(id,pb_item))
			{
				LOG_ERROR(OS_TEXT("MsgID error:")+OSString(id));
				return nullptr;
			}

			pb_item.ml->Clear();

			if(!pb_item.ml->ParseFromArray(data,size))
			{
				LOG_ERROR(OS_TEXT("Msg Parse error,MsgID:")+OSString(id));
				RETURN_FALSE;
			}

			return pb_item.func(pb_item.ml);
		}
	};//class PBMessageProc

	//使用范例
/*	class UserThread
	{
	public:

		UserThread()
		{
			mp.Init(c2s_login::id,c2s_request_all_level_info::id);								//初始化消息号

		#define SET_PB_EVENT(name)	{	\
										PBCmdFunc pcf;	\
										\
										SetEventCall(pcf,this,UserThread,on_##name);	\
										\
										mp.SetProc(c2s_##name::id,pcf,new c2s_##name);	\
									}

			SET_PB_EVENT(login);
			SET_PB_EVENT(join_game);
			SET_PB_EVENT(leave_table);

			SET_PB_EVENT(showhand);
			SET_PB_EVENT(giveup);
			SET_PB_EVENT(follow);
			SET_PB_EVENT(pass);

			SET_PB_EVENT(showcard);

			SET_PB_EVENT(request_roomlist);
			SET_PB_EVENT(request_roominfo);
			SET_PB_EVENT(requres_observe);
			SET_PB_EVENT(bet_someone);
			SET_PB_EVENT(request_next_game);

			SET_PB_EVENT(request_all_level_info);

		#undef SET_PB_EVENT
		}

		bool Update()
		{
			int32 msg_id;
			int32 msg_size;

			MemBlock<char> msg_data;

			if(!dis->ReadInt32(msg_id))RETURN_FALSE;
			if(!dis->ReadInt32(msg_size))RETURN_FALSE;

			if(msg_size<0)RETURN_FALSE;
			if(msg_size>0)
			{
				msg_data.SetLength(msg_size);
				if(dis->ReadFully(msg_data.data(),msg_size)!=msg_size)
					RETURN_FALSE;
			}

			return mp.Proc(msg_id,msg_data.data(),msg_size);
		}
	}*/
}//namespace hgl
#endif//HGL_PROTOCOLBUF_MESSAGE_PROCESS_CREATOR_INCLUDE