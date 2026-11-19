#pragma once
#include <Snooker/Rules/ShotPhysicsResult.h>
#include <Snooker/Rules/MatchState.h>

namespace Snooker
{
enum class FirstContactKind { NoContact, LegalBallOn, WrongBall };
struct FirstContactResult
{
    FirstContactKind Kind=FirstContactKind::NoContact;
    BallId CueBall=0,ContactedBall=0;
    EBallType Expected=EBallType::Red,Actual=EBallType::Red;
    bool IsFoul() const{return Kind!=FirstContactKind::LegalBallOn;}
    bool IsValid(std::string* Error=nullptr) const;
};
bool EvaluateFirstContact(const MatchState&,const ShotPhysicsResult&,FirstContactResult& Output,
                          std::string* Error=nullptr);
}
