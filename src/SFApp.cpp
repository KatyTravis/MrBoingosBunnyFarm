#include "SFApp.h"

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), timer(0), Timer(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2(canvas_w/2, 46);
  player->SetPosition(player_pos);

  youWin = make_shared<SFAsset>(SFASSET_YOUWIN, sf_window);
  auto youWinPos = Point2(canvas_w/2, canvas_h/2);
  youWin->SetPosition(youWinPos);

  Background = make_shared<SFAsset>(SFASSET_BACKGROUND, sf_window);
  auto BackgroundPos = Point2(canvas_w/2, canvas_h/2);
  Background->SetPosition(BackgroundPos);

  const int number_of_Fence = 2;
  for(int i=0; i<number_of_Fence; i++) {
    // place an alien at width/number_of_aliens * i
    auto Fence = make_shared<SFAsset>(SFASSET_FENCE, sf_window);
    auto pos   = Point2((((canvas_w/number_of_Fence)) * i ) + 120, 115.0f);
    Fence->SetPosition(pos);
    Fences.push_back(Fence);
  }

  const int number_of_aliens = 10;
  for(int i=0; i<number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2((canvas_w/number_of_aliens) * i, 100 + 200.0f);
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }

const int number_of_aliens2 = 10;
  for(int i=0; i<number_of_aliens2; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2((canvas_w/number_of_aliens2) * i, 180 + 200.0f);
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }






  auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
  auto pos  = Point2((canvas_w/4), 200);
  coin->SetPosition(pos);
  coins.push_back(coin);
}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->GoWest();
    break;
  case SFEVENT_PLAYER_RIGHT:
    player->GoEast();
    break;
  case SFEVENT_FIRE:
    fire ++;
    FireProjectile();
    break;
  case SFEVENT_PLAYER_UP:
    player->GoNorth();
    break;
  case SFEVENT_PLAYER_DOWN:
    player->GoSouth();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // wrap an SDL_Event with our SFEvent
    SFEvent sfevent((const SDL_Event) event);
    // handle our SFEvent
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {
  // Update projectile positions
  for(auto p: projectiles) {
    p->GoNorth();
  }


  for(auto c: coins) {

    if (Timer < 400 ){
       c->GoWest();
       Timer++;
      }
    
if (Timer >= 400 ){
       c->GoEast();
       Timer++;
      }
    if (Timer == 800 ){
       Timer = 0;
      }
    
  }


  // Update enemy positions
  for(auto a : aliens) {

   
   if (timer<300) {
   a->GoEastB();
   timer++ ;
   }
   if (timer>=300) {
   a->GoWestB();
   timer++ ;
   }
   if (timer==600){
   timer=0;
   }

    // do something here
  }

  // Detect collisions Projectile Vs Alien
  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
        p->HandleCollision();
        a->HandleCollision();
      }
    }
  }

 // Detect collisions Projectile Vs Fence
  for(auto p : projectiles) {
    for(auto f : Fences) {
      if(p->CollidesWith(f)) {
        p->HandleCollision();
      }
    }
  }

 // Detect collisions Projectile Vs Coin/Egg
  for(auto p : projectiles) {
    for(auto c : coins) {
      if(p->CollidesWith(c)) {
        p->HandleCollision();
        c->HandleCollision();
      }
    }
  }




  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);


// remove projectiles after collision (the long way)
  list<shared_ptr<SFAsset>> tmp3;
  for(auto p : projectiles) {
    if(p->IsAlive()) {
      tmp3.push_back(p);
    }
  }
  projectiles.clear();
  projectiles = list<shared_ptr<SFAsset>>(tmp3);


// remove dead coins (the long way)
  list<shared_ptr<SFAsset>> tmp2;
  for(auto c : coins) {
    if(c->IsAlive()) {
      tmp2.push_back(c);
    }
  }
  coins.clear();
  coins = list<shared_ptr<SFAsset>>(tmp2);
}

void SFApp::OnRender() {
  SDL_RenderClear(sf_window->getRenderer());

 Background->OnRender();  

  // draw the player
  player->OnRender();

  for(auto f: Fences) {
    if(f->IsAlive()) {f->OnRender();}
}

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender();}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender();}
  }

  for(auto c: coins) {
    c->OnRender();
  }

if (aliens.size()==0){
  youWin->OnRender();
}

  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
