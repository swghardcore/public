#ifndef UPDATECONTAINERPERMISSIONSTASK_H_
#define UPDATECONTAINERPERMISSIONSTASK_H_

#include "server/zone/objects/tangible/TangibleObject.h"
#include "server/zone/objects/creature/CreatureObject.h"

class UpdateContainerPermissionsTask : public Task {
	ManagedWeakReference<TangibleObject*> tano;
    ManagedReference<CreatureObject*> creo;
    bool perms;

public:
	UpdateContainerPermissionsTask(TangibleObject* obj, CreatureObject* creoObj = nullptr, bool removePerms = false) {
		 tano = obj;
         creo = creoObj;
         perms = removePerms;
	}

	void run() {
		Reference<TangibleObject*> tano = this->tano.get();

		if (tano == nullptr) {
			return;
		}

        if (!tano->isContainerObject())
            return;

		Locker locker(tano);

        if (perms == true) {
            tano->setContainerOwnerID(0);
        }

        else if (perms == false) {
            if (creo == nullptr) {
                return;
            }
            
            if (creo->isGrouped()) {
                tano->setContainerOwnerID(creo->getGroupID());
            }
            else {
                tano->setContainerOwnerID(creo->getObjectID());
            }
        }

	}
};

#endif /* UPDATECONTAINERPERMISSIONSTASK_H_ */