#include "pch.h"
#include "engine/utils/physical_object.h"
#include "btBulletDynamicsCommon.h"

engine::physical_object::physical_object(btRigidBody* body) : body(body) {}

//returns the forward vector of the physical object
btVector3 engine::physical_object::get_forward()
{
	btVector3 body_forward_vector;
	btTransform trans = body->getWorldTransform();
	body_forward_vector = (trans * forward_vector - trans.getOrigin());
	return body_forward_vector;
}

//Returns the up vector of the physical object
btVector3 engine::physical_object::get_up()
{
	btVector3 body_up_vector;
	btTransform trans = body->getWorldTransform();
	body_up_vector = (trans * up_vector - trans.getOrigin());
	return body_up_vector;
}

//Returns the right vector of the physical object
btVector3 engine::physical_object::get_right()
{
	btVector3 body_right_vector;
	btTransform trans = body->getWorldTransform();
	body_right_vector = (trans * right_vector - trans.getOrigin());
	return body_right_vector;
}
