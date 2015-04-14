using UnityEngine;
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
	public static float timescale = 2f;

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

	// Use this for initialization
	void Start () {
		Time.timeScale=0f;
		selected = GameObject.FindGameObjectWithTag("MainCamera").GetComponent<CameraController>().center;
		selectedExists = true;
		camRayLength = GameObject.FindGameObjectWithTag("MainCamera").GetComponent<CameraController>().camRayLength;
		lastColor = selected.GetComponentInChildren<Renderer>().material.color;
		selected.GetComponentInChildren<Renderer>().material.color = selectedColor;
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
					//Debug.Log(selected.name);
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

	// CANVAS FUNCTIONS

	public void PausePlay (){
		//Debug.Log (Time.timeScale);
		if (Time.timeScale == timescale) {
						Time.timeScale = 0;
						GameObject.FindGameObjectWithTag ("Pause").GetComponentInChildren <Text> ().text = "Play";
						mode = lastMode;
						
				}
		else {
						Time.timeScale = timescale;
						GameObject.FindGameObjectWithTag("Pause").GetComponentInChildren <Text>().text = "Pause";
						lastMode = mode;
						mode = "-";
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
				}
	}

	public void AddS (){
		if (Time.timeScale == 0) {
			GameObject piece = (GameObject)Instantiate (Stair, Stair.transform.position, Stair.transform.rotation);
			piece.GetComponent<dominoPosition> ().adding = true;
		}
	}

	public void saveState (){
		string content = "";
		GameObject[] pieces = GameObject.FindGameObjectsWithTag ("Player");
		for (int i = 0; i<pieces.Length; ++i)
						content = content + pieces [i].transform.position.x.ToString("F2") +" " +
								pieces [i].transform.position.y + " " +
								pieces [i].transform.position.z + " " + 
								pieces[i].transform.rotation.eulerAngles.y + Environment.NewLine;
		System.IO.File.WriteAllText(path, content);
	}
}
