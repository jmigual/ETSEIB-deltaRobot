using UnityEngine;
using System.Collections;

public class CameraController : MonoBehaviour {

	public GameObject center;
	public float distance=10f;
	public float distanceMin=3f;
	public float distanceMax=30f;
	public float zoomspeed=500f;
	public float theta;
	public float phi;
	public float cameraspeed=2000f;
	public float camRayLength=100f;
	public float objectiveSwitchSpeed=10f;
	public float dragSpeed=10f;
	public Color selectedColor=Color.red;

	
	private Transform ObjectiveInitial;
	private Vector3 obj_pos;
	private const float rad=2*Mathf.PI/360;
	private Vector3 ObjectiveDesired;
	private Vector2 LastMouse;
	private GameObject selected;
	private Color lastColor;
	private bool switchingobjective=false;

	void Start () {
		ObjectiveInitial=center.transform;
		selected=ObjectiveInitial.gameObject;
		lastColor=selected.renderer.material.color;
		selected.renderer.material.color=selectedColor;
		obj_pos = ObjectiveInitial.position;
		ObjectiveDesired=obj_pos;
	}

	void Update () {
		update_selected ();
		update_objective ();
		update_angles ();
		update_transform();
	}
	void update_selected (){
		if (Input.GetKeyDown (KeyCode.Mouse0)){
			Ray CamRay = Camera.main.ScreenPointToRay(Input.mousePosition);
			Debug.DrawRay (CamRay.origin, CamRay.direction);
			RaycastHit floorHit;
			if (Physics.Raycast (CamRay, out floorHit, camRayLength, 1)) {
				selected.renderer.material.color=lastColor;
				selected=floorHit.collider.gameObject;
				lastColor=selected.renderer.material.color;
				selected.renderer.material.color = selectedColor;
			}
		}
	}

	void update_objective (){
		if (Input.GetKeyDown (KeyCode.Mouse2)){
			Ray CamRay = Camera.main.ScreenPointToRay(Input.mousePosition);
			Debug.DrawRay (CamRay.origin, CamRay.direction);
			RaycastHit floorHit;
			if (Physics.Raycast (CamRay, out floorHit, camRayLength, 1)) {
				switchingobjective=true;
				center=floorHit.collider.gameObject;
			}
		}
		
		ObjectiveDesired=center.transform.position;
		if (switchingobjective){
			obj_pos=Vector3.Lerp(obj_pos, ObjectiveDesired, objectiveSwitchSpeed*Time.deltaTime);
			Vector3 error=ObjectiveDesired-obj_pos;
			if (error.magnitude<0.05)switchingobjective=false;
		}
		else{
			obj_pos=ObjectiveDesired;
		}
	}
	void update_angles (){
		distance = Mathf.Clamp(distance - Input.GetAxis("Mouse ScrollWheel")*zoomspeed*Time.deltaTime, distanceMin, distanceMax);
		if (Input.GetKey(KeyCode.A)) theta-=cameraspeed*Time.deltaTime;
		if (Input.GetKey(KeyCode.D)) theta+=cameraspeed*Time.deltaTime;
		if (Input.GetKey(KeyCode.W)) phi-=cameraspeed*Time.deltaTime;
		if (Input.GetKey(KeyCode.S)) phi+=cameraspeed*Time.deltaTime;
		if (Input.GetKeyDown (KeyCode.Mouse1)) LastMouse=Input.mousePosition;
		if (Input.GetKey (KeyCode.Mouse1)){
			Vector2 dist = Input.mousePosition;
			dist-=LastMouse;
			theta-=dist.x*dragSpeed;
			phi+=dist.y*dragSpeed;
			LastMouse=Input.mousePosition;
		}
		theta=theta%360;
		phi=Mathf.Clamp (phi, 0.05f,179.95f);
	}
	void update_transform (){
		Vector3 despl = new Vector3(Mathf.Sin(phi*rad)*Mathf.Cos(theta*rad), 
		                            Mathf.Cos(phi*rad),
		                            Mathf.Sin(phi*rad)*Mathf.Sin(theta*rad));
		despl*=distance;
		transform.position = obj_pos + despl;
		transform.rotation = Quaternion.LookRotation (-despl);
	}
}
