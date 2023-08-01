package com.telink.ble.mesh.model;

import com.telink.ble.mesh.entity.CompositionData;

import io.objectbox.converter.PropertyConverter;

public class CompositionDataConverter implements PropertyConverter<CompositionData, byte[]> {
    @Override
    public CompositionData convertToEntityProperty(byte[] databaseValue) {
        if (databaseValue == null) {
            return null;
        }
        return CompositionData.from(databaseValue);
    }

    @Override
    public byte[] convertToDatabaseValue(CompositionData entityProperty) {
        return entityProperty.raw;
    }
}