using UnityEngine;
using System.Collections;

public class calc : MonoBehaviour {
	public float r1 = 17.3f;
	public float r2 = 23.6f;
	private float x0, y0, z0,n, x, y;


	// Use this for initialization
	void Start () {
		x0 = transform.position.x;
		y0 = transform.position.y;
		z0 = transform.position.z;
		n = r2 * r2 - r1 * r1 - z0 * z0 - x0 * x0 - y0 * y0;
	}

	// Update is called once per frame
	void Update () {
		x0 = transform.position.x;
		y0 = transform.position.y;
		z0 = transform.position.z;
		n = r2 * r2 - r1 * r1 - z0 * z0 - x0 * x0 - y0 * y0;

		y = (-n*y0 + Mathf.Sqrt(n*n*y0*y0 - 4 * (x0*x0+y0*y0)* (-x0*x0*r1*r1+n*n/4)) ) / (2*(x0*x0+y0*y0));
		x = Mathf.Sqrt(r1 * r1 - y * y);
		//Debug.Log (Mathf.Atan2 (y,x)*180/Mathf.PI);

	}
}
