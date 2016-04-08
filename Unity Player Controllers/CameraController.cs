using UnityEngine;
using System.Collections;

using Helper;

public class CameraController : MonoBehaviour 
{
	private GameSettings GameSettings;

    public Transform cameraTarget;
    private float x = 0.0f;
    private float y = 0.0f;

    private int mouseXModifier = 5;
    private int mouseYModifier = 3;
	private int joystickXModifier = 7;
	private int joystickYModifier = 5;

    public float maxViewDistance = 25;  //Camera max zoom distance
    public float minViewDistance = 10;   //Camera minimum zoom distance
    public int zoomRate = 30;   //How fast the camera zooms

    private float desiredDistance = 5f;
    private float correctedDistance;


	void Start()
	{
		GameSettings = GameObject.FindGameObjectWithTag(GameTag.GameSettings).GetComponent<GameSettings>();
	}


	void LateUpdate () 
	{
	
        if(cameraTarget == null)
        {
			cameraTarget = GameObject.FindGameObjectWithTag(GameTag.Player).transform.FindChild("CameraTarget").transform;
            return;
        }
			
		if(GameSettings.ControllerState == ControllerState.Keyboard)
		{
			if(Input.GetMouseButton(PlayerInput.RightClick))
			{
				x += Input.GetAxis(PlayerInput.MouseX) * mouseXModifier;
				y -= Input.GetAxis(PlayerInput.MouseY) * mouseYModifier;

				y = ClampAngle(y, -50, 80);
			}

			desiredDistance -= Input.GetAxis(PlayerInput.ScrollWheel) * Time.deltaTime * zoomRate * Mathf.Abs(desiredDistance);     //Calculates the distance the player wants the camera to be at
		}
		else
		{
			if(Input.GetButton(PlayerInput.LBumper) && Input.GetButton(PlayerInput.RBumper))
			{
				desiredDistance += Input.GetAxis(PlayerInput.RightY) * Time.deltaTime * (zoomRate / 4) * Mathf.Abs(desiredDistance);     //Calculates the distance the player wants the camera to be at
			}
			else
			{
				x += Input.GetAxis(PlayerInput.RightX) * joystickXModifier;
				y += Input.GetAxis(PlayerInput.RightY) * joystickYModifier;

				y = ClampAngle(y, -50, 80);
			}
		}

		Quaternion rotation = Quaternion.Euler(y, x, 0);

		desiredDistance = Mathf.Clamp(desiredDistance, minViewDistance, maxViewDistance);  
		correctedDistance = desiredDistance;

        Vector3 position = cameraTarget.position - (rotation * Vector3.forward * desiredDistance);  //(x,y,z) * (0,1,0) * (angle in degrees)

        transform.rotation = rotation;
        transform.position = position;
	}

    private static float ClampAngle(float angle, float min, float max)
    {
        if(angle < -360)
            angle += 360;
        if(angle > 360)
            angle -= 360;

        return Mathf.Clamp(angle, min, max);
    }
}
