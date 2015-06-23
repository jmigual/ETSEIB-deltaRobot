using UnityEngine;
using System.Collections;

public class dominoPosition : MonoBehaviour {
	public float height;
	public bool forceApplyer;
	public bool adding = false;
	public float speed = 10f;
	
	private bool shot = false;

	void Start () {
	}

	void Update () {
		if (adding) {
					Ray camRay = Camera.main.ScreenPointToRay (Input.mousePosition);
					RaycastHit floorHit;
			
					if (Physics.Raycast (camRay, out floorHit, 100f, LayerMask.GetMask ("Floor"))) {
						transform.position = floorHit.point + height*Vector3.up;
					}
		}

		if (Input.GetKeyDown (KeyCode.Mouse0)){
						adding = false;
						
		}

		if (forceApplyer && !shot && Time.timeScale==Editor.timescale) {
			GetComponent<Rigidbody>().velocity = transform.forward * speed;
		}
	}

	void OnCollisionEnter (Collision collision){
		if (forceApplyer && collision.gameObject.tag == "Player") {
						gameObject.SetActive(false);
				}
	}
}
