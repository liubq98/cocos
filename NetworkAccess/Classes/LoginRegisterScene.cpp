#include "LoginRegisterScene.h"
#include "ui\CocosGUI.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "Utils.h"

USING_NS_CC;
using namespace cocos2d::network;
using namespace cocos2d::ui;
using namespace std;

cocos2d::Scene * LoginRegisterScene::createScene() {
  return LoginRegisterScene::create();
}

bool LoginRegisterScene::init() {
  if (!Scene::init()) {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto loginButton = MenuItemFont::create("Login", CC_CALLBACK_1(LoginRegisterScene::loginButtonCallback, this));
  if (loginButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + loginButton->getContentSize().height / 2;
    loginButton->setPosition(Vec2(x, y));
  }

  auto registerButton = MenuItemFont::create("Register", CC_CALLBACK_1(LoginRegisterScene::registerButtonCallback, this));
  if (registerButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + registerButton->getContentSize().height / 2 + 100;
    registerButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [] (Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(loginButton, registerButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  usernameInput = TextField::create("username", "arial", 24);
  if (usernameInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    usernameInput->setPosition(Vec2(x, y));
    this->addChild(usernameInput, 1);
  }

  passwordInput = TextField::create("password", "arial", 24);
  if (passwordInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 130.0f;
    passwordInput->setPosition(Vec2(x, y));
    this->addChild(passwordInput, 1);
  }

  messageBox = Label::create("", "arial", 30);
  if (messageBox) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 200.0f;
    messageBox->setPosition(Vec2(x, y));
    this->addChild(messageBox, 1);
  }

  return true;
}

void LoginRegisterScene::loginButtonCallback(cocos2d::Ref * pSender) {
  // Your code here
	string username = usernameInput->getStringValue();
	string password = passwordInput->getStringValue();
	string post = "{\"username\":\"" + username + "\"," + "\"password\":\"" + password + "\"}";

	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	request->setUrl("http://127.0.0.1:8000/auth");
	request->setResponseCallback(CC_CALLBACK_2(LoginRegisterScene::onHttpRequestCompletedLogin, this));
	request->setRequestData(post.c_str(), strlen(post.c_str()));
	request->setTag("POST Login");
	HttpClient::getInstance()->send(request);
	HttpClient::getInstance()->enableCookies(nullptr);
	request->release();
}

void LoginRegisterScene::onHttpRequestCompletedLogin(HttpClient *sender, HttpResponse *response) {
	if (!response) 
	{
		return;
	}

	auto buffer = response->getResponseData();
	rapidjson::Document doc;
	doc.Parse(buffer->data(), buffer->size());
	if (doc["status"] == true) 
	{
		messageBox->setString("Login OK");
	}
	else 
	{
		messageBox->setString(string("Login Failed\n") + doc["msg"].GetString());
	}
}


void LoginRegisterScene::registerButtonCallback(Ref * pSender) {
  // Your code here
	string username = usernameInput->getStringValue();
	string password = passwordInput->getStringValue();
	string post = "{\"username\":\"" + username + "\"," + "\"password\":\"" + password + "\"}";

	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	request->setUrl("http://127.0.0.1:8000/users");
	request->setResponseCallback(CC_CALLBACK_2(LoginRegisterScene::onHttpRequestCompletedRegister, this));
	request->setRequestData(post.c_str(), strlen(post.c_str()));
	request->setTag("POST Register");
	HttpClient::getInstance()->send(request);
	request->release();
}


void LoginRegisterScene::onHttpRequestCompletedRegister(HttpClient *sender, HttpResponse *response) {
	if (!response)
	{
		return;
	}

	auto buffer = response->getResponseData();
	rapidjson::Document doc;
	doc.Parse(buffer->data(), buffer->size());
	if (doc["status"] == true) 
	{
		messageBox->setString("Register OK");
	}
	else 
	{
		messageBox->setString(string("Register Failed\n") + doc["msg"].GetString());
	}
}
