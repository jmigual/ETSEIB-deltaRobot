using UnityEngine;
using System.Collections;

public class Draw : MonoBehaviour {
	public GameObject canvas;
	public GameObject dominoPiece;
	public float distance = 1.5f;

	private bool drawing = false;
	private Vector3 lastPosition;
	private bool first = true;

	// Use this for initialization
	void Start () {
		canvas.SetActive (false);
	}
	
	// Update is called once per frame
	void Update () {
		canvas.SetActive (false);
		if (Input.GetKeyDown (KeyCode.Mouse0)) // Començar o parar un dibuix
		{
			drawing = !drawing;
			first = true;
			lastPosition = new Vector3(100,100,100);
		}

		if (Input.GetKeyDown (KeyCode.Escape) || Input.GetKeyDown (KeyCode.Return)) // Tornar al modo editor
		{
			CameraController.onlyZoom = false;
			Camera.main.orthographic = false;
			GetComponent<Editor> ().enabled = true;

			canvas.SetActive (true);

			GetComponent<Draw> ().enabled = false;
		}

		if (Input.GetKeyDown (KeyCode.Delete)) // Eliminar tots els dominos
		{
			GameObject[] pieces = GameObject.FindGameObjectsWithTag ("Player");
			for (int i = 0; i<pieces.Length; ++i) Destroy (pieces[i]);
		}

		if (drawing) // Instantiate de dominos si estem dibuixant
		{
			Ray camRay = Camera.main.ScreenPointToRay (Input.mousePosition);
			RaycastHit floorHit;
		
			if (Physics.Raycast (camRay, out floorHit, 100f, LayerMask.GetMask ("Floor")))
			{
				if (Vector3.Distance(floorHit.point, lastPosition)>= distance)
				{
					if (!first) {
						Instantiate (dominoPiece, lastPosition+(floorHit.point-lastPosition).normalized*distance + 1.2f*Vector3.up, Quaternion.LookRotation(floorHit.point - lastPosition));
						lastPosition = lastPosition+(floorHit.point-lastPosition).normalized*distance;
					}
					else {
						first = false;
						lastPosition=floorHit.point;
					}
				}
			}
		}
	}
}
