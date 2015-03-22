﻿using UnityEngine;
using System.Collections;
using UnityEngine.UI;

public class Editor : MonoBehaviour {
	
	public Color selectedColor = Color.red;
	public GameObject X,Y,Z;
	public GameObject RX, RY, RZ;
	public float arrowlength = 10f;
	public float translateSpeed = 25f;
	public string mode = "t";
	public bool relativeRotation = true;

	private GameObject selected;
	private float camRayLength;
	private Color lastColor;
	private bool dragging=false;
	private GameObject draggingAxis,draggingRotator;
	private Vector2 LastMouse;
	private Vector3 toLastMouseR;
	private string lastMode;
	private Quaternion RXinitial, RYinitial, RZinitial;

	// Use this for initialization
	void Start () {
		Time.timeScale=0f;
		selected = GameObject.FindGameObjectWithTag("MainCamera").GetComponent<CameraController>().center;
		camRayLength = GameObject.FindGameObjectWithTag("MainCamera").GetComponent<CameraController>().camRayLength;
		lastColor = selected.renderer.material.color;
		selected.renderer.material.color = selectedColor;
		if (relativeRotation){
			RXinitial=RX.transform.rotation;
			RYinitial=RY.transform.rotation;
			RZinitial=RZ.transform.rotation;
		}
		X.GetComponentInChildren<Renderer>().material.color = Color.red;
		Y.GetComponentInChildren<Renderer>().material.color = Color.green;
		Z.GetComponentInChildren<Renderer>().material.color = Color.blue;
		RX.GetComponentInChildren<Renderer>().material.color = Color.red;
		RY.GetComponentInChildren<Renderer>().material.color = Color.green;
		RZ.GetComponentInChildren<Renderer>().material.color = Color.blue;
	}
	
	// Update is called once per frame
	void Update () {

		//bola.transform.position=pos;
		//Debug.DrawRay (RX.transform.position, Vector3.Cross(RX.transform.forward,Vector3.up).normalized*4);


		align_tools ();
		update_mode ();
		update_selected();
		align_tools ();
	}

	void align_tools (){
		X.SetActive(mode=="t");
		Y.SetActive(mode=="t");
		Z.SetActive(mode=="t");
		RX.SetActive(mode=="r");
		RY.SetActive(mode=="r");
		RZ.SetActive(mode=="r");	
		X.transform.position = selected.transform.position;
		Y.transform.position = selected.transform.position;
		Z.transform.position = selected.transform.position;
		RX.transform.position = selected.transform.position;
		RY.transform.position = selected.transform.position;
		RZ.transform.position = selected.transform.position;
		if (relativeRotation){
			RX.transform.rotation = selected.transform.rotation*RXinitial;
			RY.transform.rotation = selected.transform.rotation*RYinitial;
			RZ.transform.rotation = selected.transform.rotation*RZinitial;
		}
	}
	void update_mode (){
		//Debug.Log (mode);
		if (Time.timeScale==0){
			if (Input.GetKeyDown(KeyCode.T)){
				if (mode=="t")mode="-";
				else mode="t";
			}
			if (Input.GetKeyDown(KeyCode.R)){
				if (mode=="r")mode="-";
				else mode="r";
			}
		}
		/*if (Input.GetKeyDown(KeyCode.X)){
			Destroy (selected);
		}*/
	}
	void update_selected (){
		
		//Debug.Log(dragging);
		if (Input.GetKeyDown (KeyCode.Mouse0)){
			Ray CamRay = Camera.main.ScreenPointToRay(Input.mousePosition);
			//Debug.DrawRay (CamRay.origin, CamRay.direction);
			RaycastHit floorHit;
			if (Physics.Raycast (CamRay, out floorHit, camRayLength, 1)) {
				if (floorHit.collider.gameObject.CompareTag("Axis")){
					LastMouse = Input.mousePosition;
					dragging = true;
					char auxc = floorHit.collider.gameObject.name[0];
					if (auxc == 'X')draggingAxis = X;
					if (auxc == 'Y')draggingAxis = Y;
					if (auxc == 'Z')draggingAxis = Z;
				}
				else if (floorHit.collider.gameObject.CompareTag("Rotator")){
					Debug.Log ("Hola");
					dragging = true;
					char auxc = floorHit.collider.gameObject.name[0];
					if (auxc == 'X')draggingRotator = RX;
					if (auxc == 'Y')draggingRotator = RY;
					if (auxc == 'Z')draggingRotator = RZ;
					Ray MouseRay = Camera.main.ScreenPointToRay(Input.mousePosition);
					float t=Vector3.Dot (draggingRotator.transform.position,draggingRotator.transform.forward)-Vector3.Dot (draggingRotator.transform.forward,MouseRay.origin);
					t=t/Vector3.Dot (draggingRotator.transform.forward,MouseRay.direction);
					toLastMouseR=MouseRay.origin+t*MouseRay.direction-draggingRotator.transform.position;
				}
				else{
					selected.renderer.material.color = lastColor;
					selected = floorHit.collider.gameObject;
					lastColor = selected.renderer.material.color;
					selected.renderer.material.color = selectedColor;
				}
			}
		}
		if (Input.GetKeyUp(KeyCode.Mouse0))dragging = false;

		if (Input.GetKey (KeyCode.Mouse0) && dragging && mode=="t"){
			Vector2 despMouse = Input.mousePosition;
			despMouse -= LastMouse;
			LastMouse=Input.mousePosition;
			Vector2 axisScreenTip = Camera.main.WorldToScreenPoint(draggingAxis.transform.position+draggingAxis.transform.up*arrowlength);
			Vector2 axisScreenBase = Camera.main.WorldToScreenPoint(draggingAxis.transform.position);
			Vector2 axisScreen = axisScreenTip-axisScreenBase;
			/*Debug.Log (' ');
			Debug.Log (axisScreenTip);
			Debug.Log (axisScreenBase);
			Debug.Log ('=');
			Debug.Log (axisScreen);*/
			float res = Vector2.Dot (despMouse,axisScreen)/Mathf.Pow(axisScreen.magnitude,2);
			selected.transform.position+=draggingAxis.transform.up*res*translateSpeed;
		}
		if (Input.GetKey (KeyCode.Mouse0) && dragging && mode=="r"){
			Ray MouseRay = Camera.main.ScreenPointToRay(Input.mousePosition);
			float t=Vector3.Dot (draggingRotator.transform.position,draggingRotator.transform.forward)-Vector3.Dot (draggingRotator.transform.forward,MouseRay.origin);
			t=t/Vector3.Dot (draggingRotator.transform.forward,MouseRay.direction);
			Vector3 toPos=MouseRay.origin+t*MouseRay.direction-draggingRotator.transform.position;
			float dAngle = Vector3.Angle(toLastMouseR , toPos);
			if (Vector3.Dot (draggingRotator.transform.forward,Vector3.Cross (toLastMouseR,toPos))<0)dAngle=-dAngle;
			toLastMouseR=toPos;
			selected.transform.Rotate(draggingRotator.transform.forward*dAngle,Space.World);
		}
	}

	public void PausePlay (){
		//Debug.Log (Time.timeScale);
		if (Time.timeScale == 1) {
						Time.timeScale = 0;
						GameObject.FindGameObjectWithTag ("Pause").GetComponentInChildren <Text> ().text = "Play";
						mode = lastMode;
						
				}
		else {
						Time.timeScale = 1;
						GameObject.FindGameObjectWithTag("Pause").GetComponentInChildren <Text>().text = "Pause";
						lastMode = mode;
						mode = "-";
		}
	}
}