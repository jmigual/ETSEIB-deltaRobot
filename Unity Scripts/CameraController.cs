using UnityEngine;
using System.Collections;

public class CameraController : MonoBehaviour {


	public Transform ObjectiveInitial;
	public float distance=10f;
	public float distanceMin = 3f;
	public float distanceMax = 30f;
	public float zoomspeed=500f;
	public float theta;
	public float phi;
	public float cameraspeed=2000f;
	public float camRayLength=100f;
	public float objectiveSwitchSpeed=10f;
	public float dragSpeed=10f;

	private Vector3 obj_pos;
	private const float rad=2*Mathf.PI/360;
	private Vector3 ObjectiveDesired;
	private Vector2 LastMouse;

	void Start () {
		obj_pos = ObjectiveInitial.position;
		ObjectiveDesired=obj_pos;
	}

	void Update () {
		update_objective ();
		update_angles ();
		update_transform();
	}

	void update_objective (){
		if (Input.GetKeyDown (KeyCode.Mouse2)){
			Ray CamRay = Camera.main.ScreenPointToRay(Input.mousePosition);
			Debug.DrawRay (CamRay.origin, CamRay.direction);
			RaycastHit floorHit;
			if (Physics.Raycast (CamRay, out floorHit, camRayLength, 1)) {
				ObjectiveDesired=floorHit.collider.gameObject.transform.position;
			}
		};
		
		obj_pos=Vector3.Lerp(obj_pos, ObjectiveDesired, objectiveSwitchSpeed*Time.deltaTime);
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
