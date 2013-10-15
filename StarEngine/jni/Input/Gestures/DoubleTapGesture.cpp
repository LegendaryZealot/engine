#include "DoubleTapGesture.h"
#include "../../StarEngine/jni/Logger.h"
#include "../../StarEngine/jni/defines.h"
#include "../../StarEngine/jni/Context.h"

#include "../../StarEngine/jni/Input/InputManager.h"
#define INPUT_MANAGER (InputManager::GetInstance())

namespace star
{
	DoubleTapGesture::DoubleTapGesture(): 
		BaseGesture(),
		m_StartTime(0),
		m_TimeSinceBeginning(0),
		// [COMMENT] initialize 'm_TimeBetweenTaps'
		// [COMMENT] initialize 'm_Count' as last
		m_Count(0),
		m_StartPos(),
		m_bIsGestureOk(false)
	{

	}

	DoubleTapGesture::~DoubleTapGesture()
	{

	}

	#ifdef _WIN32
	void DoubleTapGesture::OnUpdateWinInputState()
	{
		if(INPUT_MANAGER->IsMouseButtonTapWIN(VK_LBUTTON))
		{
			m_StartTime = m_TimeSinceBeginning;
			if(m_Count < 1)
			{
				m_bIsGestureOk = true;
				m_StartPos = INPUT_MANAGER->GetCurrentMousePosition();
			}
			else
			{
				vec2 currentPos = INPUT_MANAGER->GetCurrentMousePosition();
				if(glm::abs(glm::length(m_StartPos - currentPos)) >= MAXIMUM_MOVEMENT_RANGE)
				{
					m_bIsGestureOk = false;
				}
			}
		}
		if(INPUT_MANAGER->IsMouseButtonUpWIN(VK_LBUTTON))
		{
			if(m_bIsGestureOk)
			{
				double timeSinceDown = m_TimeSinceBeginning - m_StartTime;
				if(	timeSinceDown > MINIMUM_TAP_TIME && 
					timeSinceDown < MAXIMUM_TAP_TIME && 
					m_TimeBetweenTaps < MAXIMUM_TIME_BETWEEN_TAPS)
				{
					++m_Count;
					if(m_Count >= 2)
					{
						m_Count = 0;
						m_bCompletedGesture = true;
					}
				}
				else
				{
					m_Count = 0;
				}
				m_TimeBetweenTaps = 0;
			}
			else
			{
				m_Count = 0;
			}
		}
	}
	#else
	void DoubleTapGesture::OnTouchEvent(AInputEvent* pEvent)
	{
		int32 action = AMotionEvent_getAction(pEvent);
		uint32 flags = action & AMOTION_EVENT_ACTION_MASK;
		switch(flags)
		{
		case AMOTION_EVENT_ACTION_DOWN:
			m_StartTime = m_TimeSinceBeginning;
			if(m_Count < 1)
			{
				m_bIsGestureOk = true;
				m_StartPos = INPUT_MANAGER->GetCurrentTouchPosANDR();
			}
			else
			{
				vec2 currentPos = INPUT_MANAGER->GetCurrentTouchPosANDR();
				if(glm::abs(glm::length(m_StartPos - currentPos)) >= MAXIMUM_MOVEMENT_RANGE)
				{
					m_bIsGestureOk = false;
				}
			}
			break;
		case AMOTION_EVENT_ACTION_UP:
			{
				if(m_bIsGestureOk)
				{
					double timeSinceDown = m_TimeSinceBeginning - m_StartTime;
					if(	timeSinceDown > MINIMUM_TAP_TIME &&
						timeSinceDown < MAXIMUM_TAP_TIME &&
						m_TimeBetweenTaps < MAXIMUM_TIME_BETWEEN_TAPS)
					{
						++m_Count;
						if(m_Count >= 2)
						{
							m_Count = 0;
							m_bCompletedGesture = true;
						}
					}
					else
					{
						m_Count = 0;
					}
					m_TimeBetweenTaps = 0;
				}
				else
				{
					m_Count = 0;
				}
				break;
			}
		}
	}

	#endif

	void DoubleTapGesture::Update(const Context& context)
	{
		m_TimeSinceBeginning = context.mTimeManager->GetMilliSecondsSinceStart();
		if(m_Count == 1)
		{
			m_TimeBetweenTaps += context.mTimeManager->GetMilliSeconds();
		}
	}

}
