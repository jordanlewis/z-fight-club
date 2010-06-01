#include "agent.h"
#include "Network/racerpacket.h"
#include "Network/server.h"
#include "Utilities/vec3f.h"
#include "Engine/world.h"
#include "Physics/pobject.h"
#include "Sound/sound.h"
#include "Engine/input.h"
#include "Agents/player.h"
#include "Agents/ai.h"
#include <iomanip>

unsigned int Agent::maxId = 0;    /* !<highest id number we've reached */
float        Agent::mass = AG_DEFAULT_MASS;
float        Agent::power = AG_DEFAULT_POWER;
float        Agent::maxRotate = AG_DEFAULT_MAX_ROTATE;
float        Agent::height = AG_DEFAULT_HEIGHT;
float        Agent::width = AG_DEFAULT_WIDTH;
float        Agent::depth = AG_DEFAULT_DEPTH;

/* \brief initialize a SteerInfo class
 */
SteerInfo::SteerInfo() : acceleration(0), rotation(0), weapon(RAYGUN), fire(0)
{
}


/* \brief initialize an agent class
 * \param position the agent's initial position
 */

Agent::Agent() : sound(&Sound::getInstance()), steerInfo(), kinematic(),
                 pathPosition(0), lapCounter(0)
{
    setup();
}

Agent::Agent(Vec3f position) : sound(&Sound::getInstance()), steerInfo(),
                               kinematic(position), pathPosition(0),
                               lapCounter(0)
{
    setup();
}

/* \brief initialize an agent class
 * \param position the agent's initial position
 * \param orientation the agent's initial orientation
 */
Agent::Agent(Vec3f position, float orientation)
            : sound(&Sound::getInstance()), steerInfo(),
              kinematic(position, Vec3f(0,0,0), orientation),
              pathPosition(0), lapCounter(0)
{
    setup();
}


void Agent::setup()
{
    id = maxId++;
    memset(ammo, 0, sizeof(short) * NWEAPONS);
}

/* \brief Calculate this agent's current maximum acceleration as a function
   \brief of power, mass, and current velocity.  Can return NaN.
*/
float Agent::getMaxAccel()
{
    static float pquotient = power/mass;
    return pquotient/kinematic.vel.length();
}

/* \brief get current kinematic for agent
 */
Kinematic &Agent::getKinematic ()
{
    return kinematic;
}

/* \brief get current kinematic for const agent
 */
const Kinematic &Agent::getKinematic () const
{
    return kinematic;
}

void Agent::nextLap()
{
    Scheduler &scheduler = Scheduler::getInstance();
    World &world = World::getInstance();

    sound->addSoundAt("empty.wav", GetTime(), AL_FALSE, 1.0,
                      worldObject->getPos());
    if ((world.runType == SOLO) || (world.runType == SERVER))
        {
            Weapon_t weapon = (Weapon_t) ((rand() % ((int)NWEAPONS - 1)) +1);
            ammo[weapon] += 3;
            if ((ammo[steerInfo.weapon] == 0) || steerInfo.weapon == NONE) {
                //steerInfo.weapon = weapon;
            }
            if (world.runType == SERVER){
                /*cout << "pusing weapons belonging to " 
                  << worldObject->netID << endl;*/
                if (worldObject->netID != NETOBJID_NONE)
                    Server::getInstance().pushWeapons(worldObject->netID);
            }
        }

    if (lapCounter > World::getInstance().nLaps)
    {
        /* We finished the race */
        if (scheduler.raceState < SOMEONE_DONE)
        {
            /* We finished the race first */
            scheduler.raceState = SOMEONE_DONE;
            WinnerDisplay *winner = new WinnerDisplay(Vec3f(0,0,0));
            world.winner = this;
            world.addWidget(winner);
        }
        if (this == Input::getInstance().getPlayerController().getAgent())
        {
            /* The player finished the race */
            if (scheduler.raceState < PLAYER_DONE)
                scheduler.raceState = PLAYER_DONE;
            Input::getInstance().releasePlayer();
            AIController *ai = AIManager::getInstance().control(this);
            ai->lane(0);
        }
    }
}

/* \brief Package an agent for network transfer
 */
void Agent::hton(RPAgent *payload){
    //payload->agentID = htonl(maxId);
    payload->mass = htonf(mass);
    payload->power = htonf(power);
    payload->maxRotate = htonf(maxRotate);
    payload->height = htonf(height);
    payload->width = htonf(width);
    payload->depth = htonf(depth);
    kinematic.hton(&(payload->kinematic));
    steerInfo.hton(&(payload->steerInfo));
    return;
}


/* \brief Unpackage an agent from network transfer
 */
void Agent::ntoh(RPAgent *payload){
    //maxId = payload->ntohl(payload->agentID);
    kinematic.ntoh(&(payload->kinematic));
    steerInfo.ntoh(&(payload->steerInfo));
    return;
}

void Agent::resetToTrack()
{
    World &world = World::getInstance();

    Vec3f pos = world.path.distToPoint(pathDistance);
    pos.y += 5;

    unsigned int nextKnot = world.path.closestKnotAhead(kinematic.pos);
    Vec3f toNext = world.path.knots[nextKnot] -
                   world.path.knots[(nextKnot-1) % world.path.knots.size()];

    Vec3f_t axis = {0, 1, 0};
    Quatf_t orient;
    float theta = atan2(toNext.x, toNext.z);
    FromAxisAngle(axis, theta, orient);

    static_cast<PMoveable *>(worldObject->pobject)->resetToStopped(pos,orient);
    static_cast<PMoveable *>(worldObject->pobject)->odeToKinematic();
    setKinematic(static_cast<PMoveable *>(worldObject->pobject)->outputKinematic);
}

/* \brief set kinematic for agent
 * \param kinematic a Kinematic object with the desired new parameters
 */
void Agent::setKinematic (const Kinematic &kinematic)
{
    this->kinematic = kinematic;
}

/* \brief get current steering information for agent
 */
SteerInfo &Agent::getSteering ()
{
    return steerInfo;
}

/* \brief serialize steering information for sending over the network
 * \param payload a place to write out the network-ready data
 */
void SteerInfo::hton(RPSteerInfo *payload)
{
    payload->a = htonf(acceleration);
    payload->r = htonf(rotation);
    payload->w = htonl(weapon);
    payload->f = htonl(fire);
    return;
}

/* \brief unserialize steering information received over the network
 * \param payload a place to read in data
 */
void SteerInfo::ntoh(const RPSteerInfo *payload)
{
    acceleration = ntohf(payload->a);
    rotation = ntohf(payload->r);
    weapon = static_cast<Weapon_t>(ntohl(payload->w));
    fire = ntohl(payload->f);
}

/* \brief set desired steering information for agent
 * \param steerInfo a SteerInfo object with the desired new parameters
 */
void Agent::setSteering(const SteerInfo &steerInfo)
{
    this->steerInfo = steerInfo;
}

std::ostream &operator<<(std::ostream &os, const Agent &agent)
{
    os << agent.kinematic << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, const SteerInfo &steerInfo)
{
    // acc:      10.4 rot:  -1.0 weapon: 0 fire: 0
    os << std::setprecision(1) << std::fixed
       << "acc: " << std::setw(6) << steerInfo.acceleration << " "
       << "rot: " << std::setw(4) << steerInfo.rotation << " "
       << "weapon: " << steerInfo.weapon << " "
       << "fire: " << steerInfo.fire;
    return os;
}
