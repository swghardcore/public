#include "server/zone/objects/tangible/components/SmugglerGoodsMenuComponent.h"
#include "server/zone/packets/object/ObjectMenuResponse.h"
#include "server/zone/objects/tangible/TangibleObject.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/ZoneServer.h"
#include "server/zone/ZoneProcessServer.h"
#include "server/zone/objects/tangible/Container.h"
#include "server/zone/managers/loot/LootManager.h"



void SmugglerGoodsMenuComponent::fillObjectMenuResponse(SceneObject* sceneObject, ObjectMenuResponse* menuResponse, CreatureObject* creature) const {

	TangibleObject* sceno = cast<TangibleObject*>(sceneObject);

	if (sceno == nullptr || creature == nullptr)
		return;

	menuResponse->addRadialMenuItem(20, 3, "Look inside");

	TangibleObjectMenuComponent::fillObjectMenuResponse(sceneObject, menuResponse, creature);

}

int SmugglerGoodsMenuComponent::handleObjectMenuSelect(SceneObject* sceneObject, CreatureObject* creature, byte selectedID) const {

    if (sceneObject == nullptr || creature == nullptr)
		return 0;

	if (!sceneObject->isTangibleObject() || !creature->isPlayerCreature())
		return 0;

	Zone* playerZone = creature->getZone();

	if (playerZone == nullptr)
		return 0;

	if (selectedID == 20) {
        ManagedReference<SceneObject*> inventory = creature->getSlottedObject("inventory");

        if (inventory == nullptr || inventory->isContainerFullRecursive()) {
            creature->sendSystemMessage("Your inventory is full.");
            return 0;
        }

        Reference<SceneObject*> containerSceno = creature->getZoneServer()->createObject(STRING_HASHCODE("object/tangible/container/loot/loot_crate.iff"), 1);

		if (containerSceno == nullptr)
			return 0;

		Locker clocker(containerSceno, creature);

		Container* container = dynamic_cast<Container*>(containerSceno.get());

		if (container == nullptr) {
			containerSceno->destroyObjectFromDatabase(true);
			return 0;
		}

        LootManager* lootManager = creature->getZoneServer()->getLootManager();

        if (lootManager == nullptr)
            return 0;

        ManagedReference<TangibleObject*> tano = dynamic_cast<TangibleObject*>(sceneObject);
        if (tano == nullptr) {
            return 0;
        }

        //INSTRUCTIONS - maybe this is a derp way lul
        //set maxcondition to less than 100k for normal, above 100k (exceptional), above 200k (legendary) + lootgrp number (1k-99k) + loot level (0-300), above 300k for random
        //EXAMPLES
        //3250 = normal loot, lootgrp 3, level = 250
        //110300 = exceptional loot, lootgrp 10, level = 300
        //299001 = lego loot, lootgrp 99, level = 1
        //346346 = random

        int condition = tano->getMaxCondition();

        int level = condition;

        int lootValue = condition;

        //random
        if (condition >= 300000)  {
            lootValue = System::random(2) + 1; //set to amount of lootgroups
            level = System::random (299) + 1;
            int rng = System::random(1000);
            //exceptional
            if (rng < 5)
                level += 100000;
            //legendary
            if (rng == 1)
                level += 100000;
        }
        else {
            //legendary
            if (condition >= 200000) {
                lootValue -= 200000;
            }
            //exceptional
            else if (condition >= 100000)  {
                lootValue -= 100000;
            }
            //get lootgroup number
            lootValue /= 1000.f;
        }

         
        String lootGroup = "looted_container";
         //set lootgroup depending on maxcondition
        switch (lootValue) {
            case 1:
                lootGroup = "weapons_all";
                break;
            case 2:
                lootGroup = "armor_all";
                break;
            case 3:
                lootGroup = "wearables_all";
                break;
            default:
                lootGroup = "looted_container";
        }

        TransactionLog trx(TrxCode::NPCLOOT, creature, container);


        //generating 0-3 items
		for (int i = 0; i < (System::random(3)); i++) {
			lootManager->createLoot(trx, container, lootGroup, level);
		}

		inventory->transferObject(container, -1);
		container->sendTo(creature, true);

        sceneObject->destroyObjectFromWorld(true);
		sceneObject->destroyObjectFromDatabase(true);

        return TangibleObjectMenuComponent::handleObjectMenuSelect(sceneObject, creature, selectedID);
	}

 	return TangibleObjectMenuComponent::handleObjectMenuSelect(sceneObject, creature, selectedID);

}