package ro.atlas.repository;

import org.springframework.data.mongodb.repository.MongoRepository;

import ro.atlas.entity.AtlasOwner;

public interface AtlasOwnerRepository extends MongoRepository<AtlasOwner, String> {
	AtlasOwner findByOwnerIdentity(String ownerIdentity);
}
