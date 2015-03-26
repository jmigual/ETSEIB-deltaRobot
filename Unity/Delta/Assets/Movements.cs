using UnityEngine;
using System.Collections;

public class Movements : MonoBehaviour {

	public float height=10f;
	public float radius=20f;
	public float w=3f;

	private float t=0f;

	void Update () {
		t+=Time.deltaTime;
		if ((w*t)>(2*Mathf.PI))t-=2*Mathf.PI/w;
		RobotCreator.Ocontrolled.Set (radius*Mathf.Sin (w*t),height,radius*Mathf.Cos (w*t));
	}
}
