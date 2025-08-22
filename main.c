
#include "WindowManager.h"
#include "NetworkManager.h"
#include "ThreadManager.h"
#include "VKManager.h"

int main(int argc, char *argv[]){
   
   INIT_LOG_TIMER();

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

   VkInstance vkInstance;
   VkDebugUtilsMessengerEXT vkDebugger;
   InitVolk(&vkInstance, &vkDebugger);

   Run(glfwWindow);

   if(CleanupVolk(&vkInstance, &vkDebugger))
     LOG_ERROR("Volkan Cleanup Faild");

   if(CleanupThreads(&threadData))
      LOG_ERROR("Thread Cleanup Faild");

   if(CleanupWindow(glfwWindow))
      LOG_ERROR("Window Cleanup Faild");

   if(CleanupNetwork(&conData))
     LOG_ERROR("Connection Cleanup Faild");
   
   return 0;
}