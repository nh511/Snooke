#pragma once
#include <Snooker/Physics/PocketContact.h>

namespace Snooker
{
enum class PocketState { OnCloth, AtMouth, InPocket, Potted, Ejected };
enum class PocketEvent { None, EnteredPocket, RecoveredToCloth };
struct PocketStateData
{
    PocketState State=PocketState::OnCloth;
    std::uint32_t PocketId=0;
    bool PotEventEmitted=false;
};
struct PocketStateUpdate
{
    PocketStateData State;
    PocketEvent Event=PocketEvent::None;
};
// State transition only. Physics contact/rebound is supplied by the caller.
// Bottom contact can emit EnteredPocket once; jaw/rim/slope cannot score.
bool UpdatePocketState(const PocketStateData& Previous,const PocketContact* Contact,
                       bool IsInsidePocket,bool HasReturnedToCloth,PocketStateUpdate& Output,
                       std::string* Error=nullptr);
}
