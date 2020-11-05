package com.atlas.networking;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

import org.conscrypt.Conscrypt;

import java.security.SecureRandom;
import java.security.Security;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;

import javax.net.ssl.HostnameVerifier;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSession;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;

import okhttp3.OkHttpClient;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

public class AtlasNetworkAPIFactory {

    static {
        Security.insertProviderAt(Conscrypt.newProvider(), 1);
    }

    private static OkHttpClient unsafeOkHttpClient() throws Exception {
        TrustManager[] trustAllCerts = new TrustManager[]{trustManager()};

        SSLContext sslContext = SSLContext.getInstance("SSL");
        sslContext.init(null, trustAllCerts, new SecureRandom());

        SSLSocketFactory sslSocketFactory = sslContext.getSocketFactory();

        OkHttpClient.Builder builder = new OkHttpClient.Builder();
        builder.sslSocketFactory(sslSocketFactory, (X509TrustManager) trustAllCerts[0]);
        builder.hostnameVerifier(hostnameVerifier());

        return builder.build();
    }

    private static TrustManager trustManager() {
        return new X509TrustManager() {

            @Override
            public void checkClientTrusted(X509Certificate[] x509Certificates, String s) throws CertificateException {

            }

            @Override
            public void checkServerTrusted(X509Certificate[] chain, String authType) throws CertificateException {
            }

            @Override
            public X509Certificate[] getAcceptedIssuers() {
                return new X509Certificate[]{};
            }
        };
    }

    private static HostnameVerifier hostnameVerifier() {
        return new HostnameVerifier() {

            @Override
            public boolean verify(String hostname, SSLSession session) {
                return true;
            }
        };
    }

    public static AtlasGatewayClaimAPI createGatewayClaimAPI(final String url) throws Exception {
        Gson gson = new GsonBuilder()
                .setLenient()
                .create();

        Retrofit retrofit = new Retrofit.Builder()
                .baseUrl(url)
                .addConverterFactory(GsonConverterFactory.create(gson))
                .client(unsafeOkHttpClient())
                .build();

        return retrofit.create(AtlasGatewayClaimAPI.class);
    }

    public static AtlasClientCommandAPI createClientCommandAPI(final String url) throws Exception {
        Gson gson = new GsonBuilder()
                .setLenient()
                .create();

        Retrofit retrofit = new Retrofit.Builder()
                .baseUrl(url)
                .addConverterFactory(GsonConverterFactory.create(gson))
                .client(unsafeOkHttpClient())
                .build();

        return retrofit.create(AtlasClientCommandAPI.class);
    }

    public static AtlasFirebaseAPI createFirebaseAPI(final String url) throws Exception {
        Gson gson = new GsonBuilder()
                .setLenient()
                .create();

        Retrofit retrofit = new Retrofit.Builder()
                .baseUrl(url)
                .addConverterFactory(GsonConverterFactory.create(gson))
                .client(unsafeOkHttpClient())
                .build();

        return retrofit.create(AtlasFirebaseAPI.class);
    }
}
