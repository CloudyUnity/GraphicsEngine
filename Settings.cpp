#include "Settings.h"

Settings::Settings(int index)
{
	m_generalExhibitData1.ReflectionFrameDelay = 0;
	m_generalExhibitData1.ShadowMapFramesDelay = 0;
	m_generalExhibitData1.ParticlesEnabled = true;

	m_debugData2.CameraSpeed = 10;
	m_debugData2.CameraRotationSpeed = 120;
	m_debugData2.FrustumCulling = false;
	m_debugData2.ShowShadowMap = true;
	m_debugData2.ParticlesEnabled = true;
	m_debugData2.ShadowMapFramesDelay = 0;
	m_debugData2.DebugLinesEnabled = true;
	m_debugData2.SkyboxEnabled = false;

	m_postProcessingExhibitData3.PostProcessingEnabled = true;
	m_postProcessingExhibitData3.FiltersEnabled = true;
	m_postProcessingExhibitData3.BlurEnabled = false;

	m_performanceData4.ReflectionEnabled = false;
	m_performanceData4.ShadowsEnabled = false;

	m_fogExhibitData5.FogEnabled = true;
	m_fogExhibitData5.SkyboxEnabled = false;
	m_fogExhibitData5.FreezeSkybox = true;
	m_fogExhibitData5.ShadowsEnabled = false;

	m_wireFrameData6.WireframeMode = true;

	ChangeSettings(index);
}

void Settings::ChangeSettings(int index)
{
	switch (index)
	{
	case 1:
		m_CurrentData = m_generalExhibitData1;
		break;
	case 2:
		m_CurrentData = m_debugData2;
		break;
	case 3:
		m_CurrentData = m_postProcessingExhibitData3;
		break;
	case 4:
		m_CurrentData = m_performanceData4;
		break;
	case 5:
		m_CurrentData = m_fogExhibitData5;
		break;
	case 6:
		m_CurrentData = m_wireFrameData6;
		break;
	default:
		m_CurrentData = m_generalExhibitData1;
		break;
	}
}