#include <Snooker/Physics/PocketState.h>

namespace Snooker
{
namespace { bool Fail(std::string* Error,const char* Message){if(Error)*Error=Message;return false;} }
bool UpdatePocketState(const PocketStateData& Previous,const PocketContact* Contact,
                       bool IsInsidePocket,bool HasReturnedToCloth,PocketStateUpdate& Output,std::string* Error)
{
    if(Previous.State==PocketState::Potted && HasReturnedToCloth)return Fail(Error,"potted ball cannot recover without explicit respot");
    if(Previous.State!=PocketState::OnCloth && Previous.State!=PocketState::AtMouth &&
       Previous.State!=PocketState::InPocket && Previous.State!=PocketState::Potted && Previous.State!=PocketState::Ejected)
        return Fail(Error,"invalid previous pocket state");
    if(Contact && Contact->PocketId==0)return Fail(Error,"contact must identify a pocket");
    PocketStateUpdate next;next.State=Previous;
    if(Contact && Contact->Feature==PocketFeature::Bottom && Contact->Hit) {
        if(Previous.PocketId!=0 && Previous.PocketId!=Contact->PocketId)return Fail(Error,"pocket changed during descent");
        next.State.PocketId=Contact->PocketId;next.State.State=PocketState::Potted;
        if(!Previous.PotEventEmitted) {next.State.PotEventEmitted=true;next.Event=PocketEvent::EnteredPocket;}
        else next.Event=PocketEvent::None;
    } else if(Contact && Contact->Hit) {
        if(Previous.State==PocketState::OnCloth)next.State.State=PocketState::AtMouth;
    } else if(IsInsidePocket) {
        if(Previous.State==PocketState::AtMouth || Previous.State==PocketState::OnCloth)next.State.State=PocketState::InPocket;
    } else if(HasReturnedToCloth) {
        if(Previous.State==PocketState::AtMouth || Previous.State==PocketState::InPocket || Previous.State==PocketState::Ejected) {
            next.State.State=PocketState::Ejected;
            if(Previous.State!=PocketState::Ejected) next.Event=PocketEvent::RecoveredToCloth;
            next.State.PocketId=0;next.State.PotEventEmitted=false;
        }
    }
    if(next.State.State==PocketState::Potted && HasReturnedToCloth)return Fail(Error,"potted state cannot silently eject");
    Output=next;return true;
}
}
