#pragma once
#include "LinearMath/btVector3.h"
#include "LinearMath/btMatrix3x3.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btQuickprof.h"
#include "LinearMath/btAlignedObjectArray.h"

class btBroadphaseInterface;
class btOverlappingPairCache;
class btCollisionDispatcher;
class btConstraintSolver;
struct btCollisionAlgorithmCreateFunc;
class btDefaultCollisionConfiguration;

class	btCollisionShape;
class	btDynamicsWorld;
class	btRigidBody;
class	btTypedConstraint;

namespace engine {
	/// \brief Class defining the physical objects in he bullet Dynamics World simulation. 
	class physical_object
	{
		btRigidBody* body;	//boid's rigidBody

	public:
		physical_object(btRigidBody* body);

		btRigidBody* get_body() const { return body; };	//returns object's rigidBody
		btVector3 get_forward();	//returns object's  forward direction vector
		btVector3 get_up();			//returns object's up vector
		btVector3 get_right();		//returns object's right direction vector

		void apply_forces();		//aplies drag, thrust, lift, gravity, etc.
	};
}

const btVector3 right_vector(-1, 0, 0);
const btVector3 up_vector(0, 1, 0);
const btVector3 forward_vector(0, 0, 1);
