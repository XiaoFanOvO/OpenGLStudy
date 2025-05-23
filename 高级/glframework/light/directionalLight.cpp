#include "directionalLight.h"
#include "shadow/directionalLightShadow.h"
#include "shadow/directionalLightCSMShadow.h"

DirectionalLight::DirectionalLight() {
	mShadow = new DirectionalLightCSMShadow();
}

DirectionalLight::~DirectionalLight() {

}