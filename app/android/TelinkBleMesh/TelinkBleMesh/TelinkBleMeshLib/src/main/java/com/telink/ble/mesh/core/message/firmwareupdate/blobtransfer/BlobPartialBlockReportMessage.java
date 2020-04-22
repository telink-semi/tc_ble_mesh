package com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.charset.Charset;
import java.util.ArrayList;

public class BlobPartialBlockReportMessage extends StatusMessage implements Parcelable {

    /**
     * List of chunks requested by the server
     * using UTF-8
     */
    private ArrayList<Integer> encodedMissingChunks;


    public BlobPartialBlockReportMessage() {

    }


    protected BlobPartialBlockReportMessage(Parcel in) {
        encodedMissingChunks = new ArrayList<>();
        in.readList(encodedMissingChunks, null);
    }

    public static final Creator<BlobPartialBlockReportMessage> CREATOR = new Creator<BlobPartialBlockReportMessage>() {
        @Override
        public BlobPartialBlockReportMessage createFromParcel(Parcel in) {
            return new BlobPartialBlockReportMessage(in);
        }

        @Override
        public BlobPartialBlockReportMessage[] newArray(int size) {
            return new BlobPartialBlockReportMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        encodedMissingChunks = new ArrayList<>();
        String decodeMissingChunks = new String(params, Charset.forName("UTF-8"));
        for (char c : decodeMissingChunks.toCharArray()) {
            encodedMissingChunks.add(c & 0xFFFF);
        }
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeList(encodedMissingChunks);
    }

    public ArrayList<Integer> getEncodedMissingChunks() {
        return encodedMissingChunks;
    }
}
