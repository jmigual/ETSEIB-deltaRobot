﻿using UnityEngine;
using System.Collections;
using UnityEngine.UI;
using System.IO;
using System;

public class Editor : MonoBehaviour {
	
	public Color selectedColor = Color.red;
	public GameObject X,Y,Z;
	public GameObject RX, RY, RZ;
	public float arrowlength = 10f;
	public float translateSpeed = 25f;
	public string mode = "t";
	public bool relativeRotation = true;
	public GameObject DominoPiece;
	public GameObject Force;
	public GameObject Stair;
	public string path;
	public Slider slider;
	static public float timescale = 2f;

	private GameObject selected;
	private bool selectedExists;
	private float camRayLength;
	private Color lastColor;
	private bool dragging=false;
	private GameObject draggingAxis,draggingRotator;
	private Vector2 LastMouse;
	private Vector3 toLastMouseR;
	private string lastMode;
	private Quaternion RXinitial, RYinitial, RZinitial;
	
	void Start () {
	 	Physics.gravity = 19.6f * Vector3.down;
		Time.timeScale=0f;
		selected = Camera.main.GetComponent<CameraController>().center;
		selectedExists = false;
		camRayLength = GameObject.FindGameObjectWithTag("MainCamera").GetComponent<CameraController>().camRayLength;
		if (relativeRotation){
			RXinitial=RX.transform.rotation;
			RYinitial=RY.transform.rotation;
			RZinitial=RZ.transform.rotation;
		}
		X.SetActive (true); Y.SetActive (true); Z.SetActive (true); RX.SetActive (true); RY.SetActive (true); RZ.SetActive (true);
		X.GetComponentInChildren<Renderer>().material.color = Color.red;
		Y.GetComponentInChildren<Renderer>().material.color = Color.green;
		Z.GetComponentInChildren<Renderer>().material.color = Color.blue;
		RX.GetComponentInChildren<Renderer>().material.color = Color.red;
		RY.GetComponentInChildren<Renderer>().material.color = Color.green;
		RZ.GetComponentInChildren<Renderer>().material.color = Color.blue;
	}
	

	void Update () {
		align_tools ();
		update_mode ();
		update_selected();
		align_tools ();
	}

	void align_tools (){
		X.SetActive(mode=="t" & selectedExists);
		Y.SetActive(mode=="t" & selectedExists);
		Z.SetActive(mode=="t" & selectedExists);
		RX.SetActive(mode=="r" & selectedExists);
		RY.SetActive(mode=="r" & selectedExists);
		RZ.SetActive(mode=="r" & selectedExists);	
		if (selectedExists) {
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
	}

	void update_mode (){
		if (Time.timeScale==0){
			if (Input.GetKeyDown(KeyCode.T)){
				if (mode=="t")mode="-";
				else if (selectedExists) mode="t";
			}
			if (Input.GetKeyDown(KeyCode.R)){
				if (mode=="r")mode="-";
				else if (selectedExists) mode="r";
			}
		}
	}

	void update_selected (){
		if (Input.GetKeyDown (KeyCode.Delete)) {
			Destroy (selected);
			selectedExists = false;
			mode = "-";
			Camera.main.GetComponent <CameraController>().centered = false;
		}
		if (Input.GetKeyDown (KeyCode.Mouse0)){
			Ray CamRay = Camera.main.ScreenPointToRay(Input.mousePosition);
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
					if (selectedExists) selected.GetComponentInChildren<Renderer>().material.color = lastColor;
					if (floorHit.collider.gameObject.CompareTag("EditorOnly")) selected = floorHit.collider.transform.parent.gameObject;
					else selected = floorHit.collider.gameObject;
					lastColor = selected.GetComponentInChildren<Renderer>().material.color;
					Renderer[] letsColor = selected.GetComponentsInChildren<Renderer>();
					for (int i=0; i<letsColor.Length; ++i) letsColor[i].material.color = selectedColor;
					selectedExists = true;
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
		if (Time.timeScale == timescale) {
						Time.timeScale = 0;
						GameObject.FindGameObjectWithTag ("Pause").GetComponentInChildren <Text> ().text = "Play";
						mode = lastMode;
						loadState();
				}
		else {
						if (selectedExists) selected.GetComponentInChildren<Renderer>().material.color = lastColor;
						selectedExists = false;
						Time.timeScale = timescale;
						GameObject.FindGameObjectWithTag("Pause").GetComponentInChildren <Text>().text = "Pause";
						lastMode = mode;
						mode = "-";
			
			string aux=path;
			path="./temp.df";
			saveState();
			path=aux;
		}
	}

	public void AddD (){
		if (Time.timeScale == 0) {
						GameObject piece = (GameObject)Instantiate (DominoPiece, DominoPiece.transform.position, DominoPiece.transform.rotation);
						piece.GetComponent<dominoPosition> ().adding = true;
				}
			
	}

	public void AddF (){
		if (Time.timeScale == 0) {
						GameObject piece = (GameObject)Instantiate (Force, Force.transform.position, Force.transform.rotation);
						piece.GetComponent<dominoPosition> ().adding = true;
			selected=piece;
				}
	}

	public void AddS (){
		if (Time.timeScale == 0) {
			GameObject piece = (GameObject)Instantiate (Stair, Stair.transform.position, Stair.transform.rotation);
			piece.GetComponent<dominoPosition> ().adding = true;
		}
	}

	public void LetsDraw (){
		if (selectedExists) selected.GetComponentInChildren<Renderer>().material.color = lastColor;
		selectedExists = false;
		mode = "-";
		GetComponent<Draw> ().enabled = true;
		CameraController.onlyZoom = true;
		Camera.main.orthographic = true;
		Camera.main.transform.position = new Vector3 (0, 100, 0);
		Camera.main.transform.rotation = Quaternion.LookRotation (Vector3.down);
		GetComponent<Editor> ().enabled = false;
	}

	public void TimeScale (){
		timescale = slider.value;
		Time.timeScale = slider.value;
	}

	public void saveState (){
		GameObject[] pieces = GameObject.FindGameObjectsWithTag ("Player");
		string content = pieces.Length.ToString() + Environment.NewLine;
		for (int i = 0; i<pieces.Length; ++i)
						content = content + pieces [i].transform.position.x.ToString("F2") +" " +
								pieces [i].transform.position.z.ToString("F2") + " " + 
								pieces[i].transform.rotation.eulerAngles.y.ToString("F2") + Environment.NewLine;
		System.IO.File.WriteAllText(path, content);
	}

	void loadState (){
		GameObject[] pieces = GameObject.FindGameObjectsWithTag("Player");
		foreach (GameObject piece in pieces) Destroy (piece,0.0f);
		selectedExists=false;
		StreamReader sr = new StreamReader("./temp.df");
		string fileContents = sr.ReadToEnd();
		sr.Close();
		
		string[] lines = fileContents.Split(new char[] {'\n'});
		for (int i=1; i<lines.Length-1; i++) {
			string[] vars = lines[i].Split(new char[] {' '});
			Vector3 newpos= new Vector3 (float.Parse (vars[0]),0.5f,float.Parse(vars[1]));
			Instantiate (DominoPiece, newpos+ 1.2f*DominoPiece.transform.localScale.y*Vector3.up,Quaternion.Euler(0,float.Parse(vars[2]),0));
		}
	}
}
