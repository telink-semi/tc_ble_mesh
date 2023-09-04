package com.telink.ble.mesh.model.json;

import io.objectbox.annotation.Entity;
import io.objectbox.annotation.Id;

@Entity
public class SceneRange {
    @Id
    public long id;
    public int firstScene;
    public int lastScene;

    public SceneRange() {
    }

    public SceneRange(int firstScene, int lastScene) {
        this.firstScene = firstScene;
        this.lastScene = lastScene;
    }


}