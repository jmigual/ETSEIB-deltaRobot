using UnityEngine;
using System.Collections;

public class color : MonoBehaviour {

	// Use this for initialization
	void Start () {
	}
	
	// Update is called once per frame
	void Update () {
		Vector3 translation = new Vector3();
		translation.Set(0f,0f,0.01f);
		transform.Translate(translation);
	}
}
