#include "game/AnimatedSprite.hpp"

void AnimatedSprite::animate(const sf::Time &elapsed){

    
    timeout+=elapsed.asSeconds();

    if(timeout >= 1.0/fps){

        if(frameId >= animationFrames.size()-1){
            frameId = 0;
        }
        else{
            frameId++;
        }
        timeout = 0;

    }
    setTextureRect(animationFrames[frameId]);

}

void AnimatedSprite::addAnimationFrame(const sf::IntRect &animationFrame){

    animationFrames.emplace_back(animationFrame);

}

void AnimatedSprite::setFrame(int frame_){

    frameId = frame_;

}

void AnimatedSprite::setFps(int fps_){
    fps = fps_;
}


int AnimatedSprite::getFrameId(){

    return frameId;

}

int AnimatedSprite::getFramesSize(){

    return animationFrames.size();

}
