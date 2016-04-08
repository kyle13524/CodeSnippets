using UnityEngine;
using System.Collections;
using System;

using Helper;

public class PlayerController : MonoBehaviour 
{
	private GameSettings GameSettings;

	private const float walkSpeed = 0.35f;
	private const float runSpeed = 1f;

	public float rotationSpeed;							//The speed in which our player will rotate
	public float movementSpeed;							//The speed in which our player will move

	private CameraController cameraController;			//Reference to our CameraController. Used for rotation calculations
	private Animator animator;							//Our animator that we will use to animate the player

	private float horizontalAxis;
	private float verticalAxis;

	private float calculatedCameraY;

	void Awake()
	{
		DontDestroyOnLoad(transform.gameObject);
		rotationSpeed = 200f;
		movementSpeed = 0f;
	}

	void Start()
	{
		GameSettings = GameObject.FindGameObjectWithTag(GameTag.GameSettings).GetComponent<GameSettings>();
		cameraController = GameObject.FindGameObjectWithTag(GameTag.PlayerCamera).GetComponent<CameraController>();
		animator = GetComponent<Animator>();
	}


	void Update()
	{
		float movementAxis = 0;
		float cameraY = cameraController.transform.eulerAngles.y;
		float transformY = transform.eulerAngles.y;


		//TEMPORARY AS FUCK
		if(Input.GetButtonDown("Attack"))
		{
			animator.SetTrigger("Attack");
		}


		GetControllerAxes();

		int horizontalDegreeChange = GetDegreeChange(-1, 1, (int) horizontalAxis, -90, 90);
		int verticalDegreeChange = 0;

		if(verticalAxis < 0)
		{
			verticalDegreeChange = 180;
		}
	
		if(horizontalAxis == 0 && verticalAxis == 0)
		{
			movementSpeed = 0;
		}
		else
		{
			//If both axises are not equal to 0, we are moving diagonally
			if(horizontalAxis != 0 && verticalAxis != 0)
			{
				calculatedCameraY = cameraY + ((horizontalDegreeChange / 2) + verticalDegreeChange) * verticalAxis;
			}
			else
			{
				if(horizontalAxis != 0)
				{
					movementAxis = horizontalAxis;
					calculatedCameraY = cameraY + horizontalDegreeChange;
				}
				else if(verticalAxis != 0)
				{
					movementAxis = verticalAxis;
					calculatedCameraY = cameraY + verticalDegreeChange;
				}
			}

			if(Input.GetButton(PlayerInput.Sprint))
			{
				movementSpeed = runSpeed;
			}
			else
			{
				movementSpeed = walkSpeed;
			}

		}


		if(transformY != calculatedCameraY)
		{
			transform.rotation = Quaternion.RotateTowards(transform.rotation, Quaternion.Euler(new Vector3(0, calculatedCameraY, 0)), rotationSpeed * Time.deltaTime);
		}
			
		animator.SetFloat("Speed", movementSpeed);

		//Jumping
	}


	private void GetControllerAxes()
	{
		switch(GameSettings.ControllerState)
		{
		case ControllerState.Keyboard:
			horizontalAxis = Input.GetAxisRaw(PlayerInput.Horizontal);
			verticalAxis = Input.GetAxisRaw(PlayerInput.Vertical);
			break;

		case ControllerState.XBOX:
			horizontalAxis = Input.GetAxisRaw(PlayerInput.LeftX);
			verticalAxis = -Input.GetAxisRaw(PlayerInput.LeftY);
			break;
		}

		if(horizontalAxis < 0)
			horizontalAxis = -1;
		if(horizontalAxis > 0)
			horizontalAxis = 1;
		if(verticalAxis < 0)
			verticalAxis = -1;
		if(verticalAxis > 0)
			verticalAxis = 1;
	}


	private int GetDegreeChange(int oldMin, int oldMax, int oldValue, int newMin, int newMax)
	{
		int newValue = ((oldValue - oldMin) * (newMax - newMin)) / (oldMax - oldMin) + newMin;

		return newValue;
	}


	public void PlayWalkSounds()
	{

	}
}
