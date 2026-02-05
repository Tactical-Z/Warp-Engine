
#include "WindowManager.h"
#include "NetworkManager.h"
#include "ThreadManager.h"
#include "VKManager.h"
#include "UIManager.h"

#include "Mesh.h"

#include "CppTest.h" // C++ testing function, used for compiling c with c++. Can be removed

int main(int argc, char *argv[]){
   
   INIT_LOG_TIMER();

   LOG_INFO("------- Starting Application ------");
   
   struct ThreadData* threadData = malloc(sizeof(struct ThreadData));
   if(!InitThreads(threadData))
      return 1;

   struct ConnectionData* conData = malloc(sizeof(struct ConnectionData));
   if(!InitNetwork(&argc, argv, conData))
      return 1;

   // Test switch for either client or server
   switch (connectionSide)
   {
   case CSI_SERVER:
      RunThread(threadData, 0, ServerListen, conData);
      break;
   case CSI_CLIENT:
      RunThread(threadData, 0, ClientConnect, conData);
      break;
   }

   struct GLFWwindow* glfwWindow = InitWindow(1000, 800, "C-Test");
   if(!glfwWindow)
      return 1;

   InitVolk(glfwWindow);

   InitUI(glfwWindow);

   //CPPTest_testLogFunc(); // C++ Testing

   Run(glfwWindow);

   LOG_INFO("Starting Cleanup");

   if(ShutdownUI())
      LOG_ERROR("UI Cleanup Faild");

   if(CleanupVolk())
     LOG_ERROR("Volkan Cleanup Faild");

   if(CleanupThreads(&threadData))
      LOG_ERROR("Thread Cleanup Faild");

   if(CleanupWindow(glfwWindow))
      LOG_ERROR("Window Cleanup Faild");

   if(CleanupNetwork(&conData))
     LOG_ERROR("Connection Cleanup Faild");
   
   LOG_INFO("------- Application Ended ------");

   return 0;
}