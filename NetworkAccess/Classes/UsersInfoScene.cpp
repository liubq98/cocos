#include "UsersInfoScene.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "Utils.h"

using namespace cocos2d::network;
using namespace rapidjson;
using namespace std;

cocos2d::Scene * UsersInfoScene::createScene() {
  return UsersInfoScene::create();
}

bool UsersInfoScene::init() {
  if (!Scene::init()) return false;

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto getUserButton = MenuItemFont::create("Get User", CC_CALLBACK_1(UsersInfoScene::getUserButtonCallback, this));
  if (getUserButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + getUserButton->getContentSize().height / 2;
    getUserButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(getUserButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  limitInput = TextField::create("limit", "arial", 24);
  if (limitInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    limitInput->setPosition(Vec2(x, y));
    this->addChild(limitInput, 1);
  }

  messageBox = Label::create("", "arial", 30);
  if (messageBox) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height / 2;
    messageBox->setPosition(Vec2(x, y));
    this->addChild(messageBox, 1);
  }

  return true;
}

void UsersInfoScene::getUserButtonCallback(Ref * pSender) {
  // Your code here
	string limit = limitInput->getStringValue();

	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::GET);
	request->setUrl("http://127.0.0.1:8000/users?limit=" + limit);
	request->setResponseCallback(CC_CALLBACK_2(UsersInfoScene::onHttpRequestCompleted, this));
	request->setTag("GET");
	HttpClient::getInstance()->send(request);
	request->release();
}

void UsersInfoScene::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) {
	if (!response)
	{
		return;
	}

	auto buffer = response->getResponseData();
	rapidjson::Document doc;
	doc.Parse(buffer->data(), buffer->size());

	string info;
	if (doc["status"] == true)
	{
		for (int i = 0; i < doc["data"].Size(); i++) 
		{
			info = info + "Username : " + doc["data"][i]["username"].GetString() + "\n";
			info += "Deck:\n";
			for (int j = 0; j < doc["data"][i]["deck"].Size(); j++) 
			{
				for (rapidjson::Value::ConstMemberIterator it = ((doc["data"])[i]["deck"])[j].MemberBegin(); it != ((doc["data"])[i]["deck"])[j].MemberEnd(); it++)
				{
					string name = it->name.GetString();
					int valuei = it->value.GetInt();
					stringstream stream;
					stream << valuei;
					string values = stream.str();
					info = info + "  " + name + ":" + values + "\n";
				}
				info += "  ---\n";
			}
			info += "---\n";
		}

		messageBox->setString(info);
	}
	else
	{
		messageBox->setString(string("Info Failed\n") + doc["msg"].GetString());
	}
}