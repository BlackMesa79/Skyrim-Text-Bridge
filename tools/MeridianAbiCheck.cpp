#include "../extern/MeridianUI/ViewAPI.h"
#include "MeridianApi.h"
#include <iostream>
using namespace Meridian::UI::View;
struct Mock final:IViewAPI {
 ViewHandle CreateView(const ViewCreateInfo*)override{return 0;} void DestroyView(ViewHandle)override{}
 bool IsValid(ViewHandle)const override{return true;} bool IsReady(ViewHandle)const override{return true;}
 bool RegisterListener(ViewHandle,const char*,ListenerCallback)override{return true;}
 bool ExecuteJavaScript(ViewHandle,const char*)override{return true;}
 bool Show(ViewHandle)override{return true;} bool Hide(ViewHandle)override{return true;}
 FocusResult TryFocus(ViewHandle v,FocusMode m)override{return v==123 && m==FocusMode::PauseGame?FocusResult::AlreadyFocused:FocusResult::InvalidView;}
 void Unfocus(ViewHandle)override{} bool HasFocus(ViewHandle)const override{return true;} bool HasAnyFocus()const override{return true;}
};
Mock negotiated;
bool accept=true;
bool Query(const char* name,std::uint32_t version,void** result,Meridian::UI::Settings*,const char*) {
 if(!accept || !IsSupported(name,version))return false;
 *result=&negotiated;return true;
}
int main(){Mock mock;auto table=*reinterpret_cast<void***>(&mock);using Fn=FocusResult(*)(IViewAPI*,ViewHandle,FocusMode);
 if(reinterpret_cast<Fn>(table[9])(&mock,123,FocusMode::PauseGame)!=FocusResult::AlreadyFocused)return 1;
 if(TextBridge::RequestMeridianView(Query)!=&negotiated || TextBridge::RequestMeridianView(nullptr))return 2;
 accept=false;if(TextBridge::RequestMeridianView(Query))return 3;
 std::cout<<"Meridian View/1 negotiation and MSVC x64 TryFocus slot verified\n";
}
