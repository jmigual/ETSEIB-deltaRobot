using UnityEngine;
using System.Collections;

public class Draw : MonoBehaviour {
	public GameObject canvas;
	public GameObject dominoPiece;
	public float distance = 1.5f;

	private bool drawing = false;
	private Vector3 lastPosition;
	private bool first = true;
	private GameObject lastDomino;

	void Start () {
		canvas.SetActive (false);
	}

	void Update () {
		canvas.SetActive (false);
		if (Input.GetKeyDown (KeyCode.Mouse0)) 
		{
			drawing = !drawing;
			first = true;
			lastPosition = new Vector3(100,100,100);
		}

		if (Input.GetKeyDown (KeyCode.Escape) || Input.GetKeyDown (KeyCode.Return)) 
		{
			CameraController.onlyZoom = false;
			Camera.main.orthographic = false;
			GetComponent<Editor> ().enabled = true;

			canvas.SetActive (true);

			GetComponent<Draw> ().enabled = false;
		}

		if (Input.GetKeyDown (KeyCode.Delete)) 
		{
			GameObject[] pieces = GameObject.FindGameObjectsWithTag ("Player");
			for (int i = 0; i<pieces.Length; ++i) Destroy (pieces[i]);
		}

		if (drawing) 
		{
			Ray camRay = Camera.main.ScreenPointToRay (Input.mousePosition);
			RaycastHit floorHit;
		
			if (Physics.Raycast (camRay, out floorHit, 1000f, LayerMask.GetMask ("Floor")))
			{
				if (Vector3.Distance(floorHit.point, lastPosition)>= distance)
				{
					if (!first) {
						lastDomino.transform.rotation = Quaternion.LookRotation(floorHit.point-lastPosition);
						lastDomino = Instantiate (dominoPiece, lastPosition+(floorHit.point-lastPosition).normalized*distance + 1.2f*dominoPiece.transform.localScale.y*Vector3.up, Quaternion.LookRotation(floorHit.point - lastPosition)) as GameObject;
						lastPosition = lastPosition+(floorHit.point-lastPosition).normalized*distance;
					}
					else {
						first = false;
						lastDomino = Instantiate (dominoPiece, floorHit.point + 1.2f*dominoPiece.transform.localScale.y*Vector3.up, Quaternion.LookRotation(Vector3.forward)) as GameObject;
						lastPosition=floorHit.point;
					}
				}
			}
		}
	}
}
