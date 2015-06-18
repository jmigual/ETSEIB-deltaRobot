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
		
			if (Physics.Raycast (camRay, out floorHit, 1000f, LayerMask.GetMask ("Floor")))
			{
				if (Vector3.Distance(floorHit.point, lastPosition)>= distance)
				{
					if (!first) {
						lastDomino.transform.rotation = Quaternion.LookRotation(floorHit.point-lastPosition);
						lastDomino = Instantiate (dominoPiece, lastPosition+(floorHit.point-lastPosition).normalized*distance + 1.2f*dominoPiece.transform.localScale.y*Vector3.up, Quaternion.LookRotation(floorHit.point - lastPosition)) as GameObject;
						lastPosition = lastPosition+(floorHit.point-lastPosition).normalized*distance;
						Debug.Log (floorHit.point.y);
					}
					else {
						first = false;
						lastDomino = Instantiate (dominoPiece, floorHit.point + 1.2f*dominoPiece.transform.localScale.y*Vector3.up, Quaternion.LookRotation(Vector3.forward)) as GameObject;
						Debug.Log (floorHit.point);
						lastPosition=floorHit.point;
					}
				}
			}
		}
	}
}
