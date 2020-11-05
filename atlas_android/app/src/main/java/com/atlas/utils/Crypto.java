package com.atlas.utils;

import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;

import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;

public class Crypto {
    public static byte[] computeHMAC(String algorithm, byte[] key, byte[] payload) throws NoSuchAlgorithmException, InvalidKeyException {
        Mac mac = Mac.getInstance(algorithm);
        mac.init(new SecretKeySpec(key, algorithm));

        return mac.doFinal(payload);
    }
}
