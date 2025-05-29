#include "pointLight.h"
#include "shadow/pointLightShadow.h"

PointLight::PointLight() {
	mShadow = new PointLightShadow();
}

PointLight::~PointLight() {

}