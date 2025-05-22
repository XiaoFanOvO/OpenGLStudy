#include "directionalLight.h"
#include "shadow/directionalLightShadow.h"

DirectionalLight::DirectionalLight() {
	mShadow = new DirectionalLightShadow();
}

DirectionalLight::~DirectionalLight() {

}