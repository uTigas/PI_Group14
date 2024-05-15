import { IonBackdrop, IonBadge, IonButton, IonCard, IonCardContent, IonCardHeader, IonCardSubtitle, IonCardTitle, IonCol, IonContent, IonGrid, IonHeader, IonIcon, IonInput, IonItem, IonLabel, IonPage, IonPopover, IonRouterOutlet, IonRow, IonText, IonTitle } from "@ionic/react";
import ApiWrapper from "../components/APIWrapper";
import { Route, useParams } from "react-router-dom";
import { useEffect, useState } from "react";
import { arrowBack, backspace, eyeOutline, returnDownBack, searchOutline, warning } from "ionicons/icons";
import CreateVaultContainer from "../components/CreateVaultContainer";
import "./General.css"
import AddMemberContainer from "../components/AddMemberContainer";
import Common from "../components/Common";
import { format } from "date-fns";

const Organization: React.FC = () => {
    const { id: organizationId }  = useParams<{ id: string }>();
    const [vaults, setVaults] = useState<any[]>([]);
    const [members, setMembers] = useState<any[]>([]);
    const [role, setRole] = useState<string>("");
    const [newRole, setNewRole] = useState<string>("");
    const [permissions, setPermissions] = useState<any[]>([]);
    const [organization, setOrganization] = useState<any>(null);

    useEffect(() => {
        fetchOrganization();
    }, [])

    const fetchOrganization = async() => {
        try{
        const response = await ApiWrapper.fetchOrganizationDetails(organizationId);
        if (response){
            console.log(response.data)
            setMembers(response.data.members)
            setVaults(response.data.vaults)
            setRole(response.data.role)
            setPermissions(response.data.permissions)
            setOrganization(response.data.organization)
        }
        else{
            setVaults([])
            setMembers([])
            setRole("")
            setPermissions([])
            setOrganization(null)
        }
        } catch (error){
        console.error('Error fetching Organization Details', error);
        }
    }

    const createRole = () => {
        const formData = new URLSearchParams();
        formData.append('role', newRole);
        formData.append('organization', organizationId);
        ApiWrapper.createRole(formData);
    }

    const handleGoBack = () => {
        history.back(); 
      };

    return (
        <IonPage>
            <IonContent className="ion-padding">
                <IonGrid className='grid'>
                    <IonRow>
                        <IonCol>
                            <IonButton onClick={() => handleGoBack()} shape="round" size="small" color={"warning"}>
                                <IonIcon icon={arrowBack} size="large"></IonIcon>
                                Return
                            </IonButton>
                        </IonCol>
                        <IonCol  className="ion-text-right ion-padding-end">
                            <IonText>Your Role: <IonText color="warning">{role}</IonText></IonText>
                        </IonCol>
                    </IonRow>
                    <IonRow>
                        <IonCol>
                            {organization ? (
                                <IonCard className="ion-padding">
                                    <IonCardHeader>
                                        <IonCardTitle color={"tertiary"}>{organization.name}</IonCardTitle>
                                        <IonCardSubtitle>{organization.description}</IonCardSubtitle>
                                    </IonCardHeader>
                                    <IonCardContent>
                                        {permissions.includes(Common.PERMISSIONS.MANAGE_ORGANIZATION) ?(
                                            <IonItem>
                                                <IonButton id="createRole">Create a new Organization Role</IonButton>
                                                <IonPopover  trigger="createRole" triggerAction="click">
                                                    <IonContent className="ion-padding">
                                                        <IonItem>
                                                            <IonLabel position="floating">Role Name:</IonLabel>
                                                            <IonInput
                                                            type="text"
                                                            value={newRole}
                                                            onIonChange={(e) => {
                                                                setNewRole(e.detail.value!);
                                                            }}
                                                            />
                                                        </IonItem>
                                                        <IonItem>
                                                            <IonButton expand="block" onClick={() => createRole()}>Create</IonButton>
                                                        </IonItem>
                                                    </IonContent>
                                                </IonPopover>
                                            </IonItem>
                                        ):(<></>)}
                                    </IonCardContent>
                                </IonCard>
                            ):(
                                <></>
                            )}
                        </IonCol>
                    </IonRow>
                    <IonRow className="ion-margin-vertical">
                        <IonCol size={permissions.includes(Common.PERMISSIONS.CREATE_VAULTS) || permissions.includes(Common.PERMISSIONS.MANAGE_MEMBERS)?("4"):("1")}>
                            {permissions.includes(Common.PERMISSIONS.CREATE_VAULTS) ? (
                                <>
                                     <IonItem>
                                        <IonTitle className='ion-text-start'  color={"tertiary"}>Incubator</IonTitle>
                                    </IonItem>
                                    <IonItem>
                                        <IonCard className="ion-padding" style={{width:"100%"}}>
                                            <IonCardHeader>
                                                <IonCardTitle className='title'>Create Vault:</IonCardTitle>
                                            </IonCardHeader>
                                            <IonCardContent className="ion-margin-bottom ">
                                                <CreateVaultContainer organizationId = {organizationId} fetchOrganization={fetchOrganization}/>
                                            </IonCardContent>
                                        </IonCard>
                                    </IonItem>
                                </>

                            ):(
                                <></>
                            )}
                            {permissions.includes(Common.PERMISSIONS.MANAGE_MEMBERS) ? (
                                <IonItem>
                                <IonCard className="ion-padding" style={{width:"100%"}}>
                                    <IonCardHeader>
                                        <IonCardTitle className='ion-margin-top title'>Add Member:</IonCardTitle>
                                    </IonCardHeader>
                                    <IonCardContent>
                                        <AddMemberContainer organization={organizationId} fetchOrganization={fetchOrganization}/>
                                    </IonCardContent>
                                </IonCard>
                                </IonItem>
                            ):(
                               <></>
                            )}
                        </IonCol>
                        <IonCol>
                            <IonItem>
                                <IonBadge slot="end" color={"tertiary"}>{vaults.length}</IonBadge>
                                <IonTitle color={"tertiary"}>Vaults</IonTitle>
                            </IonItem>
                            <IonItem>

                                {vaults.length !== 0 ? (
                                    vaults.map((item, index) => (
                                    <IonCard className="card organization-card ion-padding"  style={{width:"100%"}} key={item.vault.id}>
                                        <IonCardHeader>
                                            <IonCardTitle>{item.vault.name}</IonCardTitle>
                                        </IonCardHeader>
                                        <IonCardContent>
                                            <strong>{item.vault.description}</strong>
                                        </IonCardContent>
                                    </IonCard>
                                    ))
                                ):(
                                    <IonTitle className='ion-padding-vertical'>No vaults yet.</IonTitle>
                                )} 
                            </IonItem>
                        </IonCol>
                        <IonCol>
                            <IonItem>
                                <IonBadge slot="end" color={"tertiary"}>{members.length}</IonBadge>
                                <IonTitle className='ion-text-start'  color={"tertiary"}>Members</IonTitle>
                            </IonItem>
                            {members.length !== 0 ? (
                                members.map((item, index) => (
                                <IonItem>
                                    <IonCard className="card  organization-card ion-padding" style={{width:"100%"}} key={item.username}>
                                        <IonCardHeader>
                                            <IonCardTitle>{item.name}</IonCardTitle>
                                            <IonCardSubtitle color={item.role == Common.DEFAULT_ROLES.OWNER ?  ("tertiary"):(item.role == Common.DEFAULT_ROLES.ADMIN ? ("warning"):("success"))}>{item.role}</IonCardSubtitle>
                                        </IonCardHeader>
                                        <IonCardContent>
                                            <strong></strong>
                                            <IonText style={{ fontWeight: "bold", fontStyle : "italic" }}>Joined in {format(item.joined, "dd-MM-yyyy")}</IonText>
                                        </IonCardContent>
                                    </IonCard>
                                </IonItem>
                                ))
                            ):(
                                <IonTitle className='ion-padding-vertical'>No members yet.</IonTitle>
                            )} 
                        </IonCol>
                    </IonRow>
                </IonGrid>
            </IonContent>
        </IonPage>
    );
};

export default Organization;