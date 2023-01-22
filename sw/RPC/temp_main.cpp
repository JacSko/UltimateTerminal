#include <iostream>
#include "RPCServer.h"
#include "Logger.h"


int main()
{
   LoggerEngine::get()->startFrontends();
   uint32_t value = 0x11223344;

   std::vector<uint8_t> data;

   RPC::TestRequest request;
   request.cmd = RPC::Command::Test;
   request.data1 = 0x44332211;
   request.data2 = 0x55;
   request.data3 = 0x6677;

   serialize(data, request);

   printf("Serialized %x %x %x %x %x %x\n", data[0], data[1], data[2], data[3], data[4], data[5]);

   RPC::TestRequest result;
   deserialize(data, result);

   printf("CMD %u DATA1 %x DATA2 %x DATA3 %x\n", result.cmd, result.data1, result.data2, result.data3);



   RPC::RPCServer rpc;

   rpc.start(12345);

   RPC::result<RPC::TestReply> reply = rpc.invoke<RPC::TestReply, RPC::TestRequest>(result);

   if (reply.ready())
   {
      printf("OK");
   }
   else
   {
      printf("ERROR");
   }

}
