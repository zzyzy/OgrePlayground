//
// OgreGame2/Projectile.hpp
// 
// TODO Add description
// http://gameprogrammingpatterns.com/state.html
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __PROJECTILE_HPP__
#define __PROJECTILE_HPP__

#include "BulletContext.hpp"

class Projectile
{
public:
	explicit Projectile(const float& timeToLive, btRigidBody* body) :
		mTimeToLive(timeToLive),
		mLivingTime(0.0f),
		mBody(body)
	{
	}

	virtual ~Projectile()
	{
	}

	virtual void update(const float& deltaTime)
	{
		if (mLivingTime < mTimeToLive)
		{
			mLivingTime += deltaTime;
		}
	}

	virtual bool canKillself() const
	{
		if (mLivingTime >= mTimeToLive)
		{
			return true;
		}

		return false;
	}

	virtual void killMyself(Ogre::SceneManager* sceneMgr, BulletContext* bulletContext)
	{
		OnDying(sceneMgr, bulletContext);

		// Get the scene node from the motion state of the rigidbody
		Ogre::SceneNode* node = static_cast<BulletContext::OgreMotionState*>(mBody->getMotionState())->GetNode();

		// Get the entity from the node 
		// Assumes that you only have one entity, if you have more just loop through to get them all in order to delete them
		Ogre::Entity* entity = static_cast<Ogre::Entity*>(node->getAttachedObject(0));

		// Detach the entity from the scene node
		node->detachObject(entity);

		// Delete the entity and the scene node
		sceneMgr->destroyEntity(entity);
		sceneMgr->destroySceneNode(node);

		// Destroy the rigidbody from the physics system
		bulletContext->DestroyRigidBody(mBody);
	}

	virtual void OnDying(Ogre::SceneManager* sceneMgr, BulletContext* bulletContext)
	{
	}

protected:
	float mTimeToLive;
	float mLivingTime;
	btRigidBody* mBody;
};

#endif // #ifndef __PROJECTILE_HPP__
