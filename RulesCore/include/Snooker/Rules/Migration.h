#pragma once
#include <Snooker/Rules/FrameSave.h>

namespace Snooker
{
enum class MigrationStatus { Current, Migrated, RecognizedIncompatible, UnsupportedFuture, Invalid };
struct MigrationReport {MigrationStatus Status=MigrationStatus::Invalid;std::string Kind,FromVersion,ToVersion,Message;bool IsValid(std::string* Error=nullptr) const;};
bool MigrateMatchText(const std::string&,MatchState& Output,MigrationReport&,std::string* Error=nullptr);
bool MigrateSnapshotText(const std::string&,PhysicsSnapshot& Output,MigrationReport&,std::string* Error=nullptr);
bool InspectShotText(const std::string&,PhysicsShotRecord* Output,MigrationReport&,std::string* Error=nullptr);
bool InspectFrameText(const std::string&,FrameSave* Output,MigrationReport&,std::string* Error=nullptr);
}
