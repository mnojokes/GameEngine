{
	"Ball": {
		"velocity": {
			"x": 250.0,
			"y": 250.0
		},
		"direction": {
			"x": 0.0,
			"y": -1.0
		},
		"children": [],
		"components": [
            {
				"SceneComponent_0": {
					"transform": {
						"position": {
							"x": 0.0,
							"y": 0.0,
							"z": 0.0
						},
						"scale": {
							"x": 18.0,
							"y": 18.0,
							"z": 0.0
						},
						"rotation": {
							"x": 0.0,
							"y": 0.0,
							"z": 0.0
						}
					}
				}
            },
            {
				"PhysicsComponent_0": {
					"shape_type": "Circle",
					"collider_weight": 1.0
				}
            },
			{
				"GraphicsComponent_0": {
					"Material": "TransparentCircle",
					"Model": "VertexTexCoordColorQuad"
				}
			}
        ]
	}
}