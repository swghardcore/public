#ifndef DESTROYOBJECTTASK_H_
#define DESTROYOBJECTTASK_H_

#include "server/zone/objects/scene/SceneObject.h"

class DestroyObjectTask : public Task {
	ManagedWeakReference<SceneObject*> sceno;

public:
	DestroyObjectTask(SceneObject* obj) {
		 sceno = obj;
	}

	void run() {
		Reference<SceneObject*> sceno = this->sceno.get();

		if (sceno == nullptr) {
			return;
		}

		Locker locker(sceno);

		sceno->destroyObjectFromWorld(true);
		sceno->destroyObjectFromDatabase();
	}
};

#endif /* DESTROYITEMTASK_H_ */