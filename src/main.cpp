#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelAreaLayer.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>

using namespace geode::prelude;

static bool s_openGlobed = false;
static bool s_returnToCreator = false;
static bool s_returnFromTower = false;

class $modify(ShortcutsCreatorLayer, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init()) return false;

        auto menu = this->getChildByID("creator-buttons-menu");
        if (!menu) return true;

        if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("map-button"))) {
            if (auto sprite = CCSprite::createWithSpriteFrameName("GJ_mapBtn_001.png")) {
                if (auto old = button->getChildByType<CCSprite*>(0))
                    sprite->setScale(old->getScale());
                auto size = button->getContentSize();
                button->setNormalImage(sprite);
                button->setContentSize(size);
            }
            button->setEnabled(true);
            button->setTarget(this, menu_selector(ShortcutsCreatorLayer::onMap));
        }

        if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("versus-button"))) {
            if (auto sprite = CCSprite::createWithSpriteFrameName("GJ_versusBtn_001.png")) {
                if (auto old = button->getChildByType<CCSprite*>(0))
                    sprite->setScale(old->getScale());
                auto size = button->getContentSize();
                button->setNormalImage(sprite);
                button->setContentSize(size);
            }
            button->setEnabled(true);
            button->setTarget(this, menu_selector(ShortcutsCreatorLayer::onVersus));
        }

        this->scheduleOnce(schedule_selector(ShortcutsCreatorLayer::restoreMusic), 0.05f);
        return true;
    }

    void restoreMusic(float) {
        if (auto gm = GameManager::sharedState())
            gm->playMenuMusic();
    }

    void onMap(CCObject*) {
        s_returnFromTower = true;
        auto scene = LevelAreaLayer::scene();
        if (!scene) {
            s_returnFromTower = false;
            return;
        }
        CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
    }

    void onVersus(CCObject*) {
        s_openGlobed = true;
        s_returnToCreator = false;
        auto scene = MenuLayer::scene(false);
        if (!scene) {
            s_openGlobed = false;
            return;
        }
        CCDirector::sharedDirector()->replaceScene(scene);
    }
};

class $modify(ShortcutsMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        if (s_openGlobed) {
            this->setVisible(false);
            this->scheduleOnce(schedule_selector(ShortcutsMenuLayer::openGlobed), 0.0f);
        }
        return true;
    }

    void openGlobed(float) {
        if (!s_openGlobed) return;

        auto node = this->getChildByIDRecursive("dankmeme.globed2/main-menu-button");
        if (!node) {
            this->scheduleOnce(schedule_selector(ShortcutsMenuLayer::openGlobed), 0.05f);
            return;
        }

        auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
        if (!button) {
            s_openGlobed = false;
            this->setVisible(true);
            return;
        }

        s_openGlobed = false;
        s_returnToCreator = true;
        button->activate();
    }

    void onEnterTransitionDidFinish() {
        MenuLayer::onEnterTransitionDidFinish();
        if (!s_returnToCreator) return;

        s_returnToCreator = false;
        auto scene = CreatorLayer::scene();
        if (scene)
            CCDirector::sharedDirector()->replaceScene(scene);
    }
};

class $modify(ShortcutsLevelAreaLayer, LevelAreaLayer) {
    void onBack(CCObject* sender) {
        if (!s_returnFromTower) {
            LevelAreaLayer::onBack(sender);
            return;
        }

        s_returnFromTower = false;

        if (auto audio = FMODAudioEngine::sharedEngine())
            audio->stopAllMusic(true);

        auto scene = CreatorLayer::scene();
        if (scene)
            CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
    }
};