package ro.atlas.repository;

import org.springframework.data.mongodb.repository.MongoRepository;

import org.springframework.data.mongodb.repository.Query;
import ro.atlas.entity.AtlasClient;
import ro.atlas.entity.AtlasGateway;

import java.util.List;

public interface AtlasGatewayRepository extends MongoRepository<AtlasGateway, String> {
    AtlasGateway findByPsk(String psk);

    AtlasGateway findByIdentity(String identity);

    AtlasClient findClientByIdentity(String identity);

    @Query(value = "{}", fields = "{clients : 0}")
    List<AtlasGateway> findAllExcludeClients();

}
